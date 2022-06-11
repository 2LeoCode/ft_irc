/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsuardi <lsuardi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/29 15:38:31 by Leo Suardi        #+#    #+#             */
/*   Updated: 2022/06/11 14:53:15 by lsuardi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

#define BUFFER_SIZE 2048

static const timespec ping_timeout = { .tv_sec = PING_TIMEOUT, .tv_nsec = 0 };

namespace irc
{

	Server::Server( void )
	:	m_sockfd( -1 )
	{ }

	Server::~Server( void )
	{
		typedef vector< pollfd >::const_iterator	iter;

		for (iter it = m_pollfd.begin(); it != m_pollfd.end(); ++it)
			close(it->fd);
	}

	Server::Server( string name, short port, string pass, int protocol, int backlog )
	:	m_sockfd(0)
	{
		open(name, port, pass, protocol, backlog);
	}

	Server	&Server::open( string name, short port, string pass, int protocol, int backlog )
	{
#ifdef DEBUG
		init_debug();
#endif
		if (m_sockfd > 0)
			throw runtime_error("Server already opened");

		if ((m_sockfd = socket(AF_INET6, SOCK_STREAM, protocol)) == -1)
			throw runtime_error(string("socket: ") + strerror(errno));
		m_startTime = time(NULL);

		m_name = name;
		m_password = pass;
		m_execs.insert("motd", &Server::m_execMotd)
			.insert("CAP", &Server::m_execCap)
			.insert("PASS", &Server::m_execPass)
			.insert("NICK", &Server::m_execNick)
			.insert("USER", &Server::m_execUser)
			.insert("PING", &Server::m_execPing)
			.insert("OPER", &Server::m_execOper)
			.insert("KILL", &Server::m_execKill)
			.insert("MODE", &Server::m_execMode)
			.insert("JOIN", &Server::m_execJoin)
			.insert("PRIVMSG", &Server::m_execPrivmsg)
			.insert("DIE", &Server::m_execDie)
			.insert("NAMES", &Server::m_execNames)
			.insert("PONG", &Server::m_execPong)
			.insert("PART", &Server::m_execPart)
			.insert("NOTICE", &Server::m_execNotice)
			.insert("KICK", &Server::m_execKick)
			.insert("QUIT", &Server::m_execQuit);

		m_getHelpMsg();
		m_getOps();

		m_opt.reuseaddr = 1;

		if (setsockopt(m_sockfd, SOL_SOCKET, SO_REUSEADDR, &m_opt.reuseaddr, sizeof(int)))
			throw runtime_error(string("setsockopt: ") + strerror(errno));

		if (fcntl(m_sockfd, F_SETFL, O_NONBLOCK))
			throw runtime_error(string("fcntl: ") + strerror(errno));

		// Fill up the sockaddr struct for the server
		memset(&m_addr, 0, sizeof(m_addr));
		m_addr.sin6_family = AF_INET6;
		memcpy(&m_addr.sin6_addr, &in6addr_any, sizeof(in6addr_any));
		m_addr.sin6_port = htons(port);

		// Bind it to the socket
		if (bind(m_sockfd, reinterpret_cast<sockaddr*>(&m_addr), sizeof(m_addr))

		// And make socket able to receive connections and set the
		// maximum length to which the queue of pending connections
		// for sockfd may grow (backlog)
		|| listen(m_sockfd, backlog))
			throw runtime_error(string("bind/listen: ") + strerror(errno));

		// Reserve some space for future clients and
		// push the socket fd to the pollfd array used in `poll`
		m_pollfd.reserve(100);
		m_pollfd.push_back((pollfd){ m_sockfd, POLLIN, 0 });
		return *this;
	}

	void	Server::loop( void )
	{
		typedef vector< pollfd >::iterator								iter;
		typedef list< pair< int, pair< string, timespec > > >::iterator	ping_iter;

		int ret, client_fd;
		timespec cur_time;

		while (true) {
			// Poll will block until any of the sockets is readable
			ret = poll(m_pollfd.data(), m_pollfd.size(), 10);

			if (ret < 0)
				throw runtime_error(string("poll: ") + strerror(errno));

			clock_gettime(CLOCK_MONOTONIC, &cur_time);

			ping_iter it = m_pings.begin();
			while (it != m_pings.end())
			{
				if (ping_timeout < cur_time - it->second.second)
					m_kickClient(m_clients.at((it++)->first));
				else
					++it;
			}
			if (!ret)
				continue ;
			// Check which fd is rceadable by cheking pollfd::revents 
			// (if revents == POLLIN the fd is readable)

			// If we get POLLIN on the server socket, it means that a new
			// client is trying to connect
			if (m_pollfd[0].revents & POLLIN)
			{
				sockaddr_in6	addr;
				socklen_t		addr_len = sizeof(addr);

				// Accept the client and get his socket fd (client fd)
				// and store his address in `addr`
				while ((client_fd = accept(m_sockfd, (sockaddr*)&addr, &addr_len)) != -1)
				{
					// Add the client socket into the pollfd array for later poll calls
					m_pollfd.push_back(
						(pollfd) {
							.fd = client_fd,
							.events = POLLIN,
							.revents = 0
						}
					);

					// Store the client data into the server
					m_clients.insert(
						make_pair(client_fd, Client(client_fd, addr))
					);
				}
			}

			// Loop through the pollfd array
			for (iter it = m_pollfd.begin() + 1; it != m_pollfd.end(); ++it)
			{
				// If we get POLLIN on one of the clients' socket
				// it means that the client sent a request to the server,
				// so call m_recv to bufferize this request into the server
				if (it->revents == POLLIN)
				{
					ptrdiff_t offset = it - m_pollfd.begin();
					try
					{
						m_recv(it->fd);
					}
					catch (const ClientDisconnectEvent&)
					{
						it = m_pollfd.begin() + offset - 1;
					}
				}
				else if (it->revents == POLLOUT)
				{
					m_send(it->fd, m_tosend[it->fd]);
					m_tosend[it->fd].clear();
					it->events = POLLIN;
				}
			}
			// Now check if we have full commands pending,
			// and if so, execute them
			m_parsePending();
			m_execCommandQueues();

#ifdef DEBUG
			debug();
#endif
		}
	}

	string	Server::m_getMotd( void )
	{
		struct stat		fst;
		ostringstream	err;

		if (stat("config/motd.txt", &fst) == -1)
		{
			if (errno == ENOENT)
				throw MissingMotd();
			throw runtime_error(string("m_getMotd: ") + strerror(errno));
		}

		ifstream		in("config/motd.txt");
		ostringstream	s;

		if (!in)
			throw runtime_error(string("m_getMotd: ") + strerror(errno));
		s << in.rdbuf();
		return s.str();
	}

	void	Server::m_getHelpMsg( void )
	{
		ifstream		in("config/helpmsg.txt");
		ostringstream	s;

		if (!in)
			throw runtime_error(string("m_getHelpMsg: ") + strerror(errno));
		s << in.rdbuf();
		m_helpmsg = s.str();
	}

	void	Server::m_getOps( void )
	{
		ifstream		in("config/ops.list");
		string			line, name, pass;

		if (!in)
			throw runtime_error(string("m_getOps: ") + strerror(errno));
		while (getline(in, line))
		{
			size_t pos;

    		while ((pos = line.find('\'')) != string::npos)
			{
       			line.replace(pos, 1, " ");
			}

			
			istringstream is(line);

			is >> name >> pass;
			try
			{
				m_operators.insert(name.data(), pass);
			}
			catch (...)
			{ }
		}
	}

	const string	Server::m_prefix( void ) const
	{
		return ':' + m_name + ' ';
	}

	const string	Server::m_endl( void ) const
	{
		return "\r\n";
	}

	void	Server::m_recv( int fd )
	{
		static char	buf[BUFFER_SIZE + 1] = { 0 };
		int			ret;

		// Read BUFFER_SIZE bytes from the client
		ret = recv(fd, buf, BUFFER_SIZE, 0);

		// If recv returned -1 it can mean two things
		if (ret == -1)
		{
			// If errno == EWOULDBLOCK, it means that there is
			// nothing to read, but as we are in non-blocking mode
			// for the socket it returns -1 instead

			// Otherwise an error occured in recv
			if (errno != EWOULDBLOCK)
				throw runtime_error(string("recv: ") + strerror(errno));
		}

			// If recv returned 0 it means that the client closed connection
		else if (!ret)
		{
			m_kickClient(m_clients.at(fd));
			throw ClientDisconnectEvent();
		}

		// Otherwise we read something, so we append it
		// to the pending buffer
		else if (m_pending[fd].size() < 500)
		{
			buf[ret] = 0;
			m_pending[fd] += string(buf).substr(0, 500 - m_pending[fd].size());
		}
		// Loop until there is nothing to read
	}

	void	Server::m_send( int fd, const string &request )
	{
		if (send(fd, request.data(), request.size(), 0) != static_cast<int>(request.size()))
			throw runtime_error(string("send: ") + strerror(errno));
	}

	// martin ajout
	vector< string > Server::m_parseCommand( const string &rawCommand )
	{
		vector< string >		command;
		string					delimiter = " \f\r\t\b\v";
		size_t					pos(0);
		size_t					next_space;
		while (pos < rawCommand.length())
		{
			pos = rawCommand.find_first_not_of(delimiter, pos);
			if (pos == string::npos)
				break;
			if (rawCommand[pos] == ':')
			{
				command.push_back(rawCommand.substr(pos + 1, rawCommand.length() - pos + 1));
				break;
			}
			next_space = rawCommand.find_first_of(delimiter, pos);
			if (next_space == string::npos)
				next_space = rawCommand.size();
			command.push_back(rawCommand.substr(pos, next_space - pos));
			pos = next_space;
		}
		if (command.back()[command.back().length() - 1] == '\r')
			command.back().resize(command.back().length() - 1);
		return command;
	}

	// martin ajout
	int Server::m_execCommand( Client &sender, const vector< string > &command )
	{
		const char	*cmd_name = command.begin()->data();

		try
		{
			(this->*m_execs.at(cmd_name))(sender, command);
		}
		catch ( const exception &e )
		{
			ostringstream errmsg;

			errmsg << m_prefix() << ERR_UNKNOWNCOMMAND << ' ' << cmd_name << " :Unknown command" << m_endl();
			m_appendToSend(sender.sockfd, errmsg.str());
		}
		return (-1);
	}

	void	Server::m_parsePending( void )
	{
		typedef map< int, string >::iterator					Iter;

		// Call m_parseCommand on each available command in the pending buffer
		// and push the newly created command into the execute queue
		for (Iter it = m_pending.begin(); it != m_pending.end(); ++it)
		{
			size_t	found;

			while ((found = it->second.find('\n')) != string::npos)
			{
				if (found)
					m_cmds[it->first].push(m_parseCommand(it->second.substr(0, found)));
				// Erase the command from the pending buffer
				it->second.erase(0, found + 1);
			}
		}
	}

	void	Server::m_execCommandQueues( void )
	{
		typedef map< int, queue< vector< string > > >::iterator	Iter;
		bool disconnect;

		// Call m_execCommand on each command and clear
		// the execute queue
		Iter it = m_cmds.begin();
		while (it != m_cmds.end())
		{
			Iter tmp = it;
			++tmp;
			disconnect = false;
			while (!it->second.empty())
			{
				if (it->second.front().front() == "QUIT")
					disconnect = true;
				m_execCommand(m_clients.at(it->first), it->second.front());
				if (disconnect)
					break ;
				it->second.pop();
			}
			it = tmp;
		}
		m_cmds.clear();
	}

	void	Server::m_kickClient( Client &client )
	{
		typedef vector< pollfd >::iterator								iter;
		typedef list< pair< int, pair< string, timespec > > >::iterator ping_iter;


		client.exitAllChans(m_channels);
		{
			ping_iter	it = m_pings.begin();
			while (it != m_pings.end() && it->first != client.sockfd)
				++it;
			if (it != m_pings.end())
				m_pings.erase(it);
		}

		iter	it = m_pollfd.begin();

		// Search for the client inside the pollfd array
		while (it != m_pollfd.end() && it->fd != client.sockfd)
			++it;

		// Erase it totally from the server
		close(it->fd);
		m_pending.erase(it->fd);
		m_tosend.erase(it->fd);
		m_cmds.erase(it->fd);
		m_clients.erase(it->fd);
		m_pollfd.erase(it);
	}

	bool	Server::m_isLogged( const Client &c ) const
	{
		return !(c.nickname.empty() || c.username.empty()) && c.lastpass == m_password;
	}

	void	Server::m_appendToSend( int fd, const string &request )
	{
		m_tosend[fd] += request;
		for (vector<pollfd>::iterator it = m_pollfd.begin(); it != m_pollfd.end(); ++it)
			if (it->fd == fd)
				it->events = POLLOUT;
	}

	Client	&Server::m_findClient( const string &name )
	{
		for (map<int, Client>::iterator it = m_clients.begin(); it != m_clients.end(); ++it)
		{
			if (it->second.nickname == name)
				return (it->second);
		}
		throw (out_of_range("no such nickname"));
	}

	Client	&Server::m_findClientByHost( const string &hostname )
	{
		for (map<int, Client>::iterator it = m_clients.begin(); it != m_clients.end(); ++it)
		{
			if (it->second.hostname == hostname)
				return (it->second);
		}
		throw (out_of_range("no such nickname"));
	}

	const vector< string >	Server::m_make_args( int ac, ... ) const
	{
		va_list				ap;
		vector< string >	av;

		av.reserve(ac);
		va_start(ap, ac);
		while (ac--)
			av.push_back(va_arg(ap, char*));
		va_end(ap);
		return av;
	}

	void	Server::m_attributeHost( Client &user )
	{
		static unsigned	currentHostID = 0;

		for (map< string, in6_addr >::const_iterator it = m_hostnames.begin();
		it != m_hostnames.end(); ++it)
		{
			if (!memcmp(&it->second, &user.addr().sin6_addr, sizeof(in6_addr)))
			{
				user.hostname = it->first;
				return ;
			}
		}
		ostringstream	newHostName;
		newHostName << "@user" << currentHostID++;
		user.hostname = newHostName.str();
		m_hostnames.insert(make_pair(user.hostname, user.addr().sin6_addr));
	}

	void	Server::m_pingClient( Client &c )
	{
		timespec		ts;
		ostringstream	pingMsg;
		string			randstr = randomString(10);

		clock_gettime(CLOCK_MONOTONIC, &ts);
		pingMsg << m_prefix() << "PING :" << randstr << endl;
		m_pings.push_back(make_pair(c.sockfd, make_pair(randstr, ts)));
		m_appendToSend(c.sockfd, pingMsg.str());
	}

	void Server::m_welcome( Client &c )
	{
		ostringstream msg;
		msg << m_prefix() << "001 " << c.nickname << " :Welcome to the Internet Relay Chat " << c.nickname << m_endl();
		msg << m_prefix() << "002 " << c.nickname << " :Your host is KEKserv" << m_endl();
		msg << m_prefix() << "003 " << c.nickname << " :KEKserv version 1.0" << m_endl();
		msg << m_prefix() << "004 " << c.nickname << " :The server was started on " << ctime(&m_startTime) << m_endl();
		msg << m_prefix() << "005 " << c.nickname << " :CASEMAPPING=ascii CHANMODES=opsitnbv USERMODES=iwso CHANLIMIT=" << MAX_CHANNELS << " :Are supported by this server" << m_endl();
		m_appendToSend(c.sockfd, msg.str());
		m_execMotd(c, vector<string>(1, "MOTD"));
	}

}

#ifdef DEBUG
namespace irc
{

	void	adjustXY(int *x, int *y, WINDOW *win)
	{
		if (*y > getmaxy(win) - 4)
		{
			*y = 1;
			*x += 30;
		}
	}
	
	void	Server::display_info(WINDOW *win)
	{
		int x = 1;
		int y = 1;
	
		typedef map< string, Channel >::iterator chanIter;
		for (chanIter chanIt = m_channels.begin(); chanIt != m_channels.end(); chanIt++)
		{
			int chanX = x;
			mvwprintw(win, y, chanX, (*chanIt).first.c_str()); chanX += (*chanIt).first.length() + 1;
			mvwprintw(win, y, chanX, (*chanIt).second.getModes().c_str()); chanX += (*chanIt).second.getModes().length() + 1;
			x += 4; y++;
			adjustXY(&x, &y, win);
			typedef set< const irc::Client* >::const_iterator cliIter;
			for (cliIter cliIt = (*chanIt).second.users.begin();  cliIt != (*chanIt).second.users.end(); cliIt++)
			{
				adjustXY(&x, &y, win);
				int cliX = x;
				mvwprintw(win, y, cliX, (*cliIt)->nickname.c_str()); cliX += (*cliIt)->nickname.length() + 1;
				mvwprintw(win, y, cliX, (*cliIt)->hostname.c_str()); cliX += (*cliIt)->hostname.length() + 1;
				mvwprintw(win, y, cliX, (*cliIt)->getModes().c_str()); cliX += (*cliIt)->getModes().length() + 1;
				y++;
			}
			x -= 4; y++;
			adjustXY(&x, &y, win);
		}
	}
	
	void	Server::init_debug()
	{
		initscr();
		noecho();
		curs_set(0);
		cbreak();
		int yMax, xMax;
		getmaxyx(stdscr, yMax, xMax);
		win = newwin(yMax, xMax, 0, 0);
	}
	
	void	Server::debug()
	{
		wclear(win);
		box(win, 0, 0);
		display_info(win);
		wrefresh(win);
	}
}
#endif
