/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsuardi <lsuardi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/29 15:38:31 by Leo Suardi        #+#    #+#             */
/*   Updated: 2022/06/10 13:51:18 by lsuardi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include <sys/ioctl.h>
#include <cerrno>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <ctime>

#define BUFFER_SIZE 2048

#define MODE_APPLY( true_action, false_action ) \
	do\
		if (add)\
			true_action ;\
		else\
			false_action ;\
	while (0)

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
		if (m_sockfd > 0)
			throw runtime_error("Server already opened");

		if ((m_sockfd = socket(AF_INET6, SOCK_STREAM, protocol)) == -1)
			throw runtime_error(string("socket: ") + strerror(errno));
		m_startTime = time(NULL);

		m_name = name;
		m_password = pass;
		m_execs.insert("CAP", &Server::m_execCap)
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
			.insert("QUIT", &Server::m_execQuit);
			//.insert("GLOBOPS", &Server::m_execGlobops)
			//.insert("HELP", &Server::m_execHelp)
			//.insert("IMPORTMOTD", &Server::m_execImportmotd)
			//.insert("INFO", &Server::m_execInfo)
			//.insert("INVITE", &Server::m_execInvite)
			//.insert("ISBANNED", &Server::m_execIsbanned)
			//.insert("ISON", &Server::m_execIson)
			//.insert("KILLBAN", &Server::m_execKillban)
			//.insert("UNBAN", &Server::m_execUnban)
			//.insert("SHUN", &Server::m_execShun)
			//.insert("LIST", &Server::m_execList)
			//.insert("KICK", &Server::m_execKick)
			//.insert("SETNAME", &Server::m_execSetname)
			//.insert("QUIT", &Server::m_execQuit)
			//.insert("BYE", &Server::m_execQuit)
			//.insert("ME", &Server::m_execMe)
			//.insert("NOTICE", &Server::m_execNotice)
			//.insert("TIME", &Server::m_execTime)
			//.insert("TOPIC", &Server::m_execTopic)
			//.insert("USERHOST", &Server::m_execUserhost)
			//.insert("VERSION", &Server::m_execVersion)
			//.insert("WALL", &Server::m_execWall)
			//.insert("WALLOPS", &Server::m_execWallops)
			//.insert("WHO", &Server::m_execWho)
			//.insert("REHASH", &Server::m_execRehash);

		m_getMotd();
		m_getHelpMsg();
		m_getOps();

		// Disable IPV6_V6ONLY flag to allow both IPv4 and IPv6

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
		}
	}

	void	Server::m_getMotd( void )
	{
		ifstream		in("config/motd.txt");
		ostringstream	s;

		if (!in)
			throw runtime_error(string("m_getMotd: ") + strerror(errno));
		s << in.rdbuf();
		m_motd = s.str();
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

	void	Server::m_execCap( Client &sender, const vector<string> &arg )
	{
		ostringstream	response;

		if (arg.size() < 2)
			response << m_prefix() << ERR_NEEDMOREPARAMS << " * CAP :Not enough parameters" << m_endl();
		else if (arg[1] == "LS")
			response << m_prefix() << "CAP * LS :" << m_endl();
		else if (arg[1] == "END")
			return ;
		else
			response << m_prefix() << ERR_INVALIDCAPCMD << " * " << arg[1] << " :Invalid CAP sub-command" << m_endl();
		m_appendToSend(sender.sockfd, response.str());
	}

	void	Server::m_execPass( Client &sender, const vector<string> &arg )
	{
		ostringstream	response;

		if (m_isLogged(sender))
			response << m_prefix() << ERR_ALREADYREGISTRED << " * PASS :You may not reregister" << m_endl();
		else if (arg.size() < 2)
			response << m_prefix() << ERR_NEEDMOREPARAMS << " * PASS :Not enough parameters" << m_endl();
		else
		{
			sender.lastpass = arg[1];
			if (m_isLogged(sender))
			{
				m_pingClient(sender);
				m_welcome(sender);
			}
			return ;
		}
		m_appendToSend(sender.sockfd, response.str());
	}

	void	Server::m_execNick( Client &sender, const vector<string> &arg )
	{
		ostringstream	response;
		bool			initLog = m_isLogged(sender);

		if (arg.size() < 2)
			response << m_prefix() << ERR_NONICKNAMEGIVEN << " * NICK :No nickname given" << m_endl();
		else if (arg[1].length() > 9 || arg[1].find_first_not_of(g_nickCharset) != string::npos)
			response << m_prefix() << ERR_ERRONEUSNICKNAME << " * " << arg[1] << " :Erroneus nickname" << m_endl();
		else
		{
			map<int, Client>::const_iterator it = m_clients.begin();
			while (it != m_clients.end())
			{
				if (it->second.sockfd != sender.sockfd && it->second.nickname == arg[1])
				{
					response << m_prefix() << ERR_NICKNAMEINUSE << " * " << arg[1] << " :Nickname is already in use" << m_endl();
					break ;
				}
				++it;
			}
			if (it == m_clients.end())
			{
				sender.nickname = arg[1];
				if (!initLog && m_isLogged(sender))
				{
					m_pingClient(sender);
					m_welcome(sender);
				}
				return ;
			}
		}
		m_appendToSend(sender.sockfd, response.str());
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

	void	Server::m_execUser( Client &sender, const vector<string> &arg )
	{
		ostringstream	response;

		if (m_isLogged(sender))
			response << m_prefix() << ERR_ALREADYREGISTRED << " * USER :You may not reregister" << m_endl();
		else if (arg.size() < 5)
			response << m_prefix() << ERR_NEEDMOREPARAMS << " * USER :Not enough parameters" << m_endl();
		else
		{
			sender.username = arg[1];
			sender.realname = arg[4];
			if (*sender.realname.data() == ':')
				sender.realname.erase(sender.realname.begin());
			for (size_t i = 5; i < arg.size(); ++i)
				sender.realname += " " + arg[i];
			m_attributeHost(sender);
			if (m_isLogged(sender))
			{
				m_pingClient(sender);
				m_welcome(sender);
			}
			return ;
		}
		m_appendToSend(sender.sockfd, response.str());
	}

	void	Server::m_pingClient( Client &c )
	{
		timespec		ts;
		ostringstream	pingMsg;
		string			randstr = randomString(10);

		clock_gettime(CLOCK_MONOTONIC, &ts);
		pingMsg << m_prefix() << " PING :" << randstr << endl;
		m_pings.push_back(make_pair(c.sockfd, make_pair(randstr, ts)));
		m_appendToSend(c.sockfd, pingMsg.str());
	}

	void	Server::m_execPing( Client &sender, const vector<string> &arg )
	{
		ostringstream	response;

		if (arg.size() < 2)
			response << m_prefix() << ERR_NEEDMOREPARAMS << " * PING :Not enough parameters" << m_endl();
		else if (!m_isLogged(sender))
			response << m_prefix() << ERR_NOTREGISTERED << " * PING :You have not registered" << m_endl();
		else
			response << m_prefix() << "PONG :" << arg[1] << m_endl();
		m_appendToSend(sender.sockfd, response.str());
	}

	void	Server::m_execOper ( Client &sender, const vector<string> &arg )
	{
		ostringstream	response;

		if (arg.size() < 3)
			response << m_prefix() << ERR_NEEDMOREPARAMS << " * OPER :Not enough parameters" << m_endl();
		else if (!m_isLogged(sender))
			response << m_prefix() << ERR_NOTREGISTERED << " * OPER :You have not registered" << m_endl();
		else if (m_operators.at(arg[1].data()) != arg[2])
			response << m_prefix() << ERR_PASSWDMISMATCH << " * OPER :Password incorrect" << m_endl();
		else
		{
			sender.addMode(UMODE_OPERATOR);
			response << m_prefix() << RPL_YOUREOPER << " * OPER :You are now an IRC operator" << m_endl();
		}
		m_appendToSend(sender.sockfd, response.str());
	}

	void	Server::m_execMode( Client &sender, const vector<string> &arg )
	{
		static map< char, int >	userModes = INIT_USERMODES(),
								channelModes = INIT_CHANNELMODES();
		bool					add = true;
		ostringstream			response;

		if (arg.size() < 2)
			response << m_prefix() << ERR_NEEDMOREPARAMS << " * MODE :Not enough parameters" << m_endl();
		else if (!m_isLogged(sender))
			response << m_prefix() << ERR_NOTREGISTERED << " * MODE :You have not registered" << m_endl();
		else
		{
			string::const_iterator it;

			if (arg.size() > 2)
			{
				it = arg[2].begin();
				if (*it == '-' || *it == '+')
					add = (*it++ == '+');
			}
			if (*arg[1].data() == '&' || *arg[1].data() == '#')
			{
				// CHANNEL MODES
				Channel	*chan;

				try
				{
					chan = &m_channels.at(arg[1]);
				}
				catch (const exception& e)
				{
					response << m_prefix() << ERR_NOSUCHCHANNEL << ' ' << arg[1] << " :No such channel" << m_endl();
					goto END;
				}
				if (!sender.hasMode(UMODE_OPERATOR) && chan->users.find(&sender) == chan->users.end())
				{
					response << m_prefix() << ERR_NOTONCHANNEL << ' ' << arg[1] << " :You're not on that channel" << m_endl();
					goto END;
				}
				if (arg.size() == 2)
				{
					response << m_prefix() << RPL_CHANNELMODEIS << ' ' << arg[1] << " :" << chan->getModes() << m_endl();
					goto END;
				}
				if (chan->isOperator(sender) || sender.hasMode(UMODE_OPERATOR))
				{
					int	nextArg = 3;

					while (it != arg[2].end())
					{
						Client			*user;
						bool			banByHost = false;
						const string	*key;
						size_t			limit;

						if (strchr("olbvk", *it))
						{
							if (arg.size() <= (size_t)nextArg)
							{
								response << m_prefix() << ERR_NEEDMOREPARAMS << " * MODE :Not enough parameters" << m_endl();
								++it;
								continue ;
							}
							if (*it == 'k')
							{
								if (add)
									key = &arg[nextArg++];
							}
							else if (*it == 'l')
							{
								if (add)
								{
									istringstream	is(arg[nextArg++]);
									is >> limit;
									if (!is.eof() || !limit)
									{
										++it;
										continue ;
									}
								}
							}
							else if (*arg[nextArg].data() == '@')
							{
								try
								{
									user = &m_findClientByHost(arg[nextArg++]);
								}
								catch (...)
								{
									++it;
									continue ;
								}
								banByHost = true;
							}
							else
							{
								try
								{
									user = &m_findClient(arg[nextArg++]);
								}
								catch (...)
								{
									response << m_prefix() << ERR_NOSUCHNICK << arg[nextArg - 1] << " :No such nick" << m_endl();
									++it;
									continue ;
								}
							}
						}
						switch (*it)
						{
							case 'o':
								MODE_APPLY(chan->op(*user), chan->deop(*user));
								response << user->makePrefix() << "MODE " << chan->name << ' ' << (add ? '+' : '-') << 'o';
								break ;
							case 'l':
								MODE_APPLY(chan->userLimit = limit, chan->userLimit = 0);
								response << user->makePrefix() << "MODE " << chan->name << ' ' << (add ? '+' : '-') << 'l';
								if (add) response << ' ' << chan->userLimit;
								response << m_endl();
								break ;
							case 'b':
								response << user->makePrefix() << "MODE " << chan->name << ' ' << (add ? '+' : '-') << "b ";
								if (banByHost)
								{
									MODE_APPLY(chan->banHostname(user->hostname), chan->unbanHostname(user->hostname));
									response << user->hostname;
								}
								else
								{
									MODE_APPLY(chan->banNickname(user->nickname), chan->unbanNickname(user->nickname));
									response << user->nickname;
								}
								response << m_endl();
								break ;
							case 'v':
								MODE_APPLY(chan->voice(*user), chan->unvoice(*user));
								response << user->makePrefix() << "MODE " << chan->name << ' ' << (add ? '+' : '-') << "v " << user->nickname << m_endl();
								break ;
							case 'k':
								MODE_APPLY(chan->password = *key, chan->password.clear());
								response << user->makePrefix() << "MODE " << chan->name << ' ' << (add ? '+' : '-') << 'k';
								if (add) response << ' ' << *key;
								response << m_endl();
								break ;
							default:
								try
								{
									MODE_APPLY(chan->addMode(channelModes.at(*it)), chan->delMode(channelModes.at(*it)));
									response << user->makePrefix() << "MODE " << chan->name << ' ' << (add ? '+' : '-') << *it << m_endl();
								}
								catch (...)
								{
									response << m_prefix() << ERR_UNKNOWNMODE << ' ' << *it << " :is unknown mode char to me" << m_endl();
								}
						}
						++it;
					}
				}
			}
			else
			{
				// USER MODES
				Client	*user;

				try
				{
					user = &m_findClient(arg[1]);
				}
				catch (...)
				{
					response << m_prefix() << ERR_NOSUCHNICK << ' ' << arg[1] << " :No such nick" << m_endl();
					goto END;
				}
				if (arg.size() == 2)
				{
					response << user->makePrefix() << RPL_UMODEIS << ' ' << arg[1] << " :" << user->getModes() << m_endl();
					goto END;	
				}
				while (it != arg[2].end())
				{
					if (!sender.hasMode(UMODE_OPERATOR))
					{
						if (*it == 'o')
							response << m_prefix() << ERR_NOPRIVILEGES << " * MODE :Permission Denied- You're not an IRC operator" << m_endl();
						else if (arg[1] != sender.nickname)
							response << m_prefix() << ERR_USERSDONTMATCH << " * MODE :Cant change mode for other users" << m_endl();
					}
					try
					{
						MODE_APPLY(user->addMode(userModes.at(*it)), user->delMode(userModes.at(*it)));
						response << user->makePrefix() << "MODE " << user->nickname << ' ' << (add ? '+' : '-') << *it << m_endl();
					}
					catch (...)
					{
						response << m_prefix() << ERR_UNKNOWNMODE << ' ' << *it << " :is unknown mode char to me" << m_endl();
					}
					++it;
				}
			}
		}

		END:
		m_appendToSend(sender.sockfd, response.str());
	}

	void	Server::m_execDie( Client &sender, const vector<string> &arg )
	{
		ostringstream response;

		(void)arg;
		if (!m_isLogged(sender))
			response << m_prefix() << ERR_NOTREGISTERED << " * DIE :You have not registered" << m_endl();
		else if (!sender.hasMode(UMODE_OPERATOR))
			response << m_prefix() << ERR_NOPRIVILEGES << " * :Permission Denied- You're not an IRC operator" << m_endl();
		else
			throw (ShutdownEvent());
		m_appendToSend(sender.sockfd, response.str());
	}

	void	Server::m_execKill( Client &sender, const vector<string> &arg )
	{
		ostringstream response;

		if (arg.size() < 2)
			response << m_prefix() << ERR_NEEDMOREPARAMS << " * KILL :Not enough parameters" << m_endl();
		else if (!m_isLogged(sender))
			response << m_prefix() << ERR_NOTREGISTERED << " * KILL :You have not registered" << m_endl();
		else if (!sender.hasMode(UMODE_OPERATOR))
			response << m_prefix() << ERR_NOPRIVILEGES << " * KILL :Permission Denied- You're not an IRC operator" << m_endl();
		else if (arg[1] == m_name)
			response << m_prefix() << ERR_CANTKILLSERVER << " * KILL :you cant kill a server!" << m_endl(); 
		else
		{
			try
			{
				m_kickClient(m_findClient(arg[1]));
			}
			catch (...)
			{
				response << m_prefix() << ERR_NOSUCHNICK << " * KILL :" << arg[1] << "No such nick/channel" << m_endl();
			}
		}
		m_appendToSend(sender.sockfd, response.str());
	}

	void		Server::m_execJoin( Client &sender, const vector<string> &arg )
	{
		vector< string >					chanNames;
		vector< string >					keys;
		vector< string >::const_iterator	currentKey;
		queue< Channel* >					chansToJoin;
		ostringstream 						response;

		if (arg.size() < 2)
			response << m_prefix() << ERR_NEEDMOREPARAMS << " * JOIN :Not enough parameters" << m_endl();
		else if (!m_isLogged(sender))
			response << m_prefix() << ERR_NOTREGISTERED << " * JOIN :You have not registered" << m_endl();
		else
		{
			chanNames = split(arg[1], ',');
			if (arg.size() > 2)
				keys = split(arg[2], ',');
			currentKey = keys.begin();
			typedef vector<string>::const_iterator Iter;
			for (Iter it = chanNames.begin(); it != chanNames.end(); ++it)
			{
				if (*it->data() != '#' && *it->data() != '&')
				{
					response << m_prefix() << ERR_NOSUCHCHANNEL << ' ' << *it << " :No such channel" << m_endl();
					continue ;
				}
				try
				{
					chansToJoin.push(&m_channels.at(*it));
				}
				catch (...)
				{
					if (*it->data() == '&' && !sender.hasMode(UMODE_OPERATOR))
						response << m_prefix() << ERR_NOSUCHCHANNEL << ' ' << *it << " :No such channel" << m_endl();
					else
					{
						m_channels.insert(make_pair(*it, Channel(*it)));
						chansToJoin.push(&m_channels.at(*it));
					}
				}
			}
			while (!chansToJoin.empty())
			{
				Channel	*cur = chansToJoin.front();
				chansToJoin.pop();

				if (sender.channelCount() == MAX_CHANNELS)
				{
					response << ERR_TOOMANYCHANNELS << ' ' << cur->name << " :You have joined to many channels" << m_endl();
					continue ;
				}
				if (cur->hasMode(CMODE_INVITEONLY) && !sender.isInvited(*cur))
				{
					response << m_prefix() << ERR_INVITEONLYCHAN << ' ' << cur->name << " :Cannot join channel (+i)" << m_endl();
					continue ;
				}
				if (cur->isBanned(sender))
				{
					response << m_prefix() << ERR_BANNEDFROMCHAN << ' ' << cur->name << " :Cannot join channel (+b)" << m_endl();
					continue ;
				}
				if (!cur->password.empty())
				{
					if (currentKey == keys.end())
						goto BAD_KEY;
					if (*currentKey != cur->password)
					{
						BAD_KEY:
						response << m_prefix() << ERR_BADCHANNELKEY << ' ' << cur->name << " :Cannot join channel (+k)" << m_endl();
						continue ;
					}
				}
				if (cur->userLimit != 0 && cur->users.size() == cur->userLimit)
				{
					response << m_prefix() << ERR_CHANNELISFULL << ' ' << cur->name << " :Cannot join channel (+l)" << m_endl();
					continue ;
				}
				sender.joinChannel(*cur);
				if (cur->users.size() == 1)
					cur->op(sender);
				response << m_prefix() << RPL_TOPIC << ' ' << cur->name << " :" << cur->topic << m_endl();
				m_appendToSend(sender.sockfd, response.str());

				ostringstream info;
				info << sender.makePrefix() << "JOIN :" << cur->name << m_endl();
				typedef set< const Client* >::const_iterator Iter2;
				for (Iter2 it2 = cur->users.begin(); it2 != cur->users.end(); ++it2)
					m_appendToSend((*it2)->sockfd, info.str());
				m_execNames(sender, m_make_args(2, "NAMES", cur->name.data()));
				return ;
			}
		}
		m_appendToSend(sender.sockfd, response.str());
	}

	// Reste a coder l'envoi du message et a checker la validite des clients avant de les push sur les vecteurs de targets.
	void	Server::m_execPrivmsg(Client &sender, const vector<string> &arg)
	{
		ostringstream response;
		vector< const Client* > privTargets;
		vector< pair < const Client*, const string > > chanTargets;
		

		if (!m_isLogged(sender))
		{
			response << m_prefix() << " * PRIVMSG :You have not registered" << m_endl();
		}
		else if (arg.size() == 1)
		{
			response << m_prefix() << ERR_NORECIPIENT << " * PRIVMSG :No recipient given (PRIVMSG)" << m_endl();
		}
		else if (arg.size() == 2)
		{
			if (arg[1][0] == ':')
			{
				response << m_prefix() << ERR_NORECIPIENT << " * PRIVMSG :No recipient given (PRIVMSG)" << m_endl();
			}
			else
			{
				response << m_prefix() << ERR_NOTEXTTOSEND << " * PRIVMSG :No text to send (PRIVMSG)" << m_endl();
			}
		}
		else if (arg[2][0] != ':')
		{
			response << m_prefix() << ERR_NOTEXTTOSEND << " * PRIVMSG :No text to send (PRIVMSG)" << m_endl();
		}
		else
		{
			typedef vector< string >::iterator iter;
			vector< string > receivers = split(arg[1], ',');
			for (iter it = receivers.begin(); it != receivers.end(); it++)
			{
				if (*it->data() == '#' || *it->data() == '&')
				{
					//push every client of channel to chanTargets
					try
					{
						typedef set< const irc::Client* >::iterator cliIter;
						Channel *ptr = &m_channels.at(*it);
						if (ptr->canSpeak(sender))
						{
							for (cliIter cliIt = ptr->users.begin(); cliIt != ptr->users.end(); cliIt++)
							{
								if ((*cliIt)->sockfd != sender.sockfd)
									chanTargets.push_back(make_pair(*cliIt, ptr->name));
							}
						}
						else
						{
							response << m_prefix() << ERR_CANNOTSENDTOCHAN << ptr->name << " :Cannot send to channel" << m_endl();
						}
					}
					catch (...)
					{
						response << m_prefix() << ERR_NOSUCHNICK << *it << " :No such nick/channel" << m_endl();
					}
				}
				else
				{
					privTargets.push_back(&m_findClient(*it));
				}
			}
			//loop through target
			typedef vector< const irc::Client* >::iterator privIter;
			for (privIter privIt = privTargets.begin(); privIt != privTargets.end(); privIt++)
			{
				string request = sender.makePrefix() + arg[0] + " " + (*privIt)->nickname + " " + arg[2] + m_endl();
				m_appendToSend((*privIt)->sockfd, request);
			}
			typedef vector< pair < const irc::Client*, const string > >::iterator chanIter;
			for (chanIter chanIt = chanTargets.begin(); chanIt != chanTargets.end(); chanIt++)
			{
				string request = sender.makePrefix() + arg[0] + " " + chanIt->second + " " + arg[2] + m_endl();
				m_appendToSend(chanIt->first->sockfd, request);
				// send message
			}
		}
		m_appendToSend(sender.sockfd, response.str());
	}

	void	Server::m_execNames( Client &sender, const vector< string > &arg )
	{
		typedef set< const Client* >::const_iterator	Iter;
		ostringstream						response;
		vector< string >					channels;
		vector< string >::const_iterator	curChannel;

		if (arg.size() < 2)
			response << m_prefix() << ERR_NEEDMOREPARAMS << " * NAMES :Not enough parameters" << m_endl();
		else if (!m_isLogged(sender))
			response << m_prefix() << ERR_NOTREGISTERED << " * NAMES :You have not registered" << m_endl();
		else
		{
			channels = split(arg[1], ',');
			curChannel = channels.begin();
			while (curChannel != channels.end())
			{
				const Channel	*c;
				try
				{
					c = &m_channels.at(*curChannel);
				}
				catch (...)
				{ continue ; }
				response << m_prefix() << RPL_NAMREPLY << ' ' << sender.nickname << " = " << *curChannel << " :";
				Iter it = c->users.begin();
				while (it != c->users.end())
				{
					if (c->isOperator(**it))
						response << '@';
					response << (*it)->nickname;
					if (++it != c->users.end())
						response << ' ';
				}
				response << m_endl() << m_prefix() << RPL_ENDOFNAMES << ' ' << sender.nickname << ' ' << *curChannel << " :End of /NAMES list" << m_endl();
				++curChannel;
			}
		}
		m_appendToSend(sender.sockfd, response.str());
	}

	void Server::m_execPart( Client &sender, const vector< string > &arg )
	{
		ostringstream		response;
		vector< string >	channels;

		if (arg.size() < 2)
			response << m_prefix() << ERR_NEEDMOREPARAMS << " * PART :Not enough parameters" << m_endl();
		else if (!m_isLogged(sender))
			response << m_prefix() << ERR_NOTREGISTERED << " * PART :You have not registered" << m_endl();
		else
		{
			channels = split(arg[1], ',');
			for (size_t i = 0; i < channels.size(); i++)
			{
				map< string, Channel >::iterator it = m_channels.find(channels[i]);
				if (it == m_channels.end())
					response << m_prefix() << ERR_NOSUCHCHANNEL << ' ' << channels[i] << " :No such channel" << m_endl();
				else if (!it->second.hasClient(sender))
					response << m_prefix() << ERR_NOTONCHANNEL << ' ' << channels[i] << " :You're not on that channel" << m_endl();
				else
				{
					ostringstream info;
					info << sender.makePrefix() << "PART :" << it->second.name << m_endl();
					typedef set< const Client* >::iterator	Iter;
					for (Iter it2 = it->second.users.begin(); it2 != it->second.users.end(); ++it2)
						m_appendToSend((*it2)->sockfd, info.str());
					sender.partChannel(it->second);
					// EXIT CHANNEL
					if (it->second.empty())
						m_channels.erase(it);
						// Erase channel if no one is on it
				}
			}
		}
		m_appendToSend(sender.sockfd, response.str());
	}

	void Server::m_execPong( Client &sender, const vector< string > &arg )
	{
		ostringstream		response;
		typedef list< pair< int, pair< string, timespec > > >::iterator iter;

		if (arg.size() < 2)
			response << m_prefix() << ERR_NEEDMOREPARAMS << " * PONG :Not enough parameters" << m_endl();
		else
		{
			iter it = m_pings.begin();
			while (it != m_pings.end() && it->first != sender.sockfd)
				++it;
			if (it != m_pings.end())
			{
				if (it->second.first == arg[1])
					m_pings.erase(it);
			}
			return ;
		}
		m_appendToSend(sender.sockfd, response.str());
	}

	void Server::m_execQuit( Client &sender, const vector< string > &arg )
	{
		ostringstream response;

		(void)arg;
		if (!m_isLogged(sender))
			response << m_prefix() << ERR_NOTREGISTERED << " * QUIT :You have not registered" << m_endl();
		else
		{
			m_kickClient(sender);
			return ;
		}
		m_appendToSend(sender.sockfd, response.str());
	}

	void Server::m_welcome( const Client &c )
	{
		ostringstream msg;
		msg << m_prefix() << "001 " << c.nickname << " :Welcome to the Internet Relay Chat " << c.nickname << m_endl();
		msg << m_prefix() << "002 " << c.nickname << " :Your host is KEKserv" << m_endl();
		msg << m_prefix() << "003 " << c.nickname << " :KEKserv version 1.0" << m_endl();
		msg << m_prefix() << "004 " << c.nickname << " :The server was started on " << ctime(&m_startTime) << m_endl();
		msg << m_prefix() << "005 " << c.nickname << " :CASEMAPPING=ascii CHANMODES=opsitnbv USERMODES=iwso :Are supported by this server" << m_endl();
		m_appendToSend(c.sockfd, msg.str());
	}

}
