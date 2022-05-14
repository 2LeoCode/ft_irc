/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsuardi <lsuardi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/29 15:38:31 by Leo Suardi        #+#    #+#             */
/*   Updated: 2022/05/14 14:56:55 by lsuardi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include <sys/ioctl.h>
#include <cerrno>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>

#define BUFFER_SIZE 2048

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
		if (m_sockfd)
			throw std::runtime_error("Server already opened");

		if ((m_sockfd = socket(AF_INET6, SOCK_STREAM, protocol)) == -1)
			throw std::runtime_error(string("socket: ") + strerror(errno));

		m_name = name;
		m_password = pass;
		m_execs.insert("CAP", &Server::m_execCap)
			.insert("PASS", &Server::m_execPass)
			.insert("NICK", &Server::m_execNick)
			.insert("USER", &Server::m_execUser)
			.insert("PING", &Server::m_execPing);
			//.insert("PONG", &Server::m_execPong);
			//.insert("OPER", &Server::m_execOper)
			//.insert("DIE", &Server::m_execDie)
			//.insert("GLOBOPS", &Server::m_execGlobops)
			//.insert("HELP", &Server::m_execHelp)
			//.insert("IMPORTMOTD", &Server::m_execImportmotd)
			//.insert("INFO", &Server::m_execInfo)
			//.insert("INVITE", &Server::m_execInvite)
			//.insert("ISBANNED", &Server::m_execIsbanned)
			//.insert("ISON", &Server::m_execIson)
			//.insert("KILL", &Server::m_execKill)
			//.insert("KILLBAN", &Server::m_execKillban)
			//.insert("UNBAN", &Server::m_execUnban)
			//.insert("SHUN", &Server::m_execShun)
			//.insert("LIST", &Server::m_execList)
			//.insert("MODE", &Server::m_execMode)
			//.insert("JOIN", &Server::m_execJoin)
			//.insert("KICK", &Server::m_execKick)
			//.insert("SETNAME", &Server::m_execSetname)
			//.insert("PART", &Server::m_execPart)
			//.insert("PRIVMSG", &Server::m_execPrivmsg)
			//.insert("QUIT", &Server::m_execQuit)
			//.insert("BYE", &Server::m_execQuit)
			//.insert("ME", &Server::m_execMe)
			//.insert("NOTICE", &Server::m_execNotice)
			//.insert("NAMES", &Server::m_execNames)
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
			throw std::runtime_error(string("setsockopt: ") + strerror(errno));

		const int flags = fcntl(m_sockfd, F_GETFL);
		if (fcntl(m_sockfd, F_SETFL, flags | O_NONBLOCK))
			throw std::runtime_error(string("fcntl: ") + strerror(errno));

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
			throw std::runtime_error(string("bind/listen: ") + strerror(errno));

		// Reserve some space for future clients and
		// push the socket fd to the pollfd array used in `poll`
		m_pollfd.reserve(100);
		m_pollfd.push_back((pollfd){ m_sockfd, POLLIN, 0 });
		return *this;
	}

	void	Server::loop( void )
	{
		typedef vector< pollfd >::iterator	iter;

		int ret, client_fd;

		while (true) {
			// Poll will block until any of the sockets is readable
			ret = poll(m_pollfd.data(), m_pollfd.size(), -1);

			if (ret < 0)
				throw std::runtime_error(string("poll: ") + strerror(errno));

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
						std::make_pair(client_fd, Client(client_fd, addr))
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
			throw std::runtime_error(string("m_getMotd: ") + strerror(errno));
		s << in.rdbuf();
		m_motd = s.str();
	}

	void	Server::m_getHelpMsg( void )
	{
		ifstream		in("config/helpmsg.txt");
		ostringstream	s;

		if (!in)
			throw std::runtime_error(string("m_getHelpMsg: ") + strerror(errno));
		s << in.rdbuf();
		m_helpmsg = s.str();
	}

	void	Server::m_getOps( void )
	{
		ifstream		in("config/ops.list");
		string			line, name, pass;

		if (!in)
			throw std::runtime_error(string("m_getOps: ") + strerror(errno));
		while (std::getline(in, line))
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
				throw std::runtime_error(string("recv: ") + strerror(errno));
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
			throw std::runtime_error(string("send: ") + strerror(errno));
	}

	// martin ajout
	vector< string > Server::m_parseCommand(const string &rawCommand)
	{
		vector< string >	command;
		string				delimiter = " ";
		size_t				pos(0);

		while (pos < rawCommand.length())
		{
			command.push_back(rawCommand.substr(pos, rawCommand.find(delimiter, pos) - pos));
			pos = rawCommand.find(delimiter, pos);
			pos += (pos != rawCommand.npos);
		}
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
		catch ( const std::exception &e )
		{
			cerr << e.what() << endl;
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

		// Call m_execCommand on each command and clear
		// the execute queue
		for (Iter it = m_cmds.begin(); it != m_cmds.end(); ++it)
		{
			while (!it->second.empty())
			{
				m_execCommand(m_clients.at(it->first), it->second.front());
				it->second.pop();
			}
		}
		m_cmds.clear();
	}

	void	Server::m_kickClient( Client &client )
	{
		typedef vector< pollfd >::iterator	iter;

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

	void	Server::m_execCap( Client &sender, const vector<string> &arg )
	{
		ostringstream	response;

		response << ':' << m_name << ' ';
		if (arg.size() < 2)
			response << ERR_NEEDMOREPARAMS << " * CAP :Not enough parameters";
		else if (arg[1] == "LS")
			response << "CAP * LS :";
		else if (arg[1] == "END")
			return ;
		else
			response << ERR_INVALIDCAPCMD << " * " << arg[1] << " :Invalid CAP sub-command";
		response << "\r\n";
		m_appendToSend(sender.sockfd, response.str());
	}

	void	Server::m_execPass( Client &sender, const vector<string> &arg )
	{
		ostringstream	response;

		response << ':' << m_name << ' ';
		if (m_isLogged(sender))
			response << ERR_ALREADYREGISTRED << " * PASS :You may not reregister";
		else if (arg.size() < 2)
			response << ERR_NEEDMOREPARAMS << " * PASS :Not enough parameters";
		else
		{
			sender.lastpass = arg[1];
			return ;
		}
		response << "\r\n";
		m_appendToSend(sender.sockfd, response.str());
	}

	void	Server::m_execNick( Client &sender, const vector<string> &arg )
	{
		ostringstream	response;

		response << ':' << m_name << ' ';
		if (arg.size() < 2)
			response << ERR_NONICKNAMEGIVEN << " * NICK :No nickname given";
		else if (arg[1].length() > 9 || arg[1].find_first_not_of(g_nickCharset) != string::npos)
			response << ERR_ERRONEUSNICKNAME << " * " << arg[1] << " :Erroneus nickname";
		else
		{
			map<int, Client>::const_iterator it = m_clients.begin();
			while (it != m_clients.end())
			{
				if (it->second.sockfd != sender.sockfd && it->second.nickname == arg[1])
				{
					response << ERR_NICKNAMEINUSE << " * " << arg[1] << " :Nickname is already in use";
					break ;
				}
				++it;
			}
			if (it == m_clients.end())
			{
				sender.nickname = arg[1];
				return ;
			}
		}
		response << "\r\n";
		m_appendToSend(sender.sockfd, response.str());
	}

	void	Server::m_execUser( Client &sender, const vector<string> &arg )
	{
		ostringstream	response;

		response << ':' << m_name << ' ';
		if (m_isLogged(sender))
			response << ERR_ALREADYREGISTRED << " * USER :You may not reregister";
		else if (arg.size() < 5)
			response << ERR_NEEDMOREPARAMS << " * USER :Not enough parameters";
		else
		{
			sender.username = arg[1];
			sender.realname = arg[4];
			if (*sender.realname.data() == ':')
				sender.realname.erase(sender.realname.begin());
			for (size_t i = 5; i < arg.size(); ++i)
				sender.realname += " " + arg[i];
			return ;
		}
		response << "\r\n";
		m_appendToSend(sender.sockfd, response.str());
	}

	void	Server::m_execPing( Client &sender, const vector<string> &arg )
	{
		ostringstream	response;

		response << ':' << m_name << ' ';
		if (arg.size() < 2)
			response << ERR_NEEDMOREPARAMS << " * PING :Not enough parameters";
		else if (!m_isLogged(sender))
			response << ERR_NOTREGISTERED << " * PING :You have not registered";
		else
		{
			response << "PONG :" << arg[1];
		}
		response << "\r\n";
		m_appendToSend(sender.sockfd, response.str());
	}

	void	Server::m_execOper ( Client &sender, const vector<string> &arg )
	{
		ostringstream	response;

		response << ':' << m_name << ' ';
		if (arg.size() < 3)
			response << ERR_NEEDMOREPARAMS << " * OPER :Not enough parameters";
		else if (!m_isLogged(sender))
			response << ERR_NOTREGISTERED << " * OPER :You have not registered";
		else if (m_operators.at(arg[1].data()) != arg[2])
			response << ERR_PASSWDMISMATCH << " * OPER :Password incorrect";
		else
		{
			sender.addMode(UMODE_OPERATOR);
			response << RPL_YOUREOPER << " * OPER :You are now an IRC operator";
		}
		response << "\r\n";
		m_appendToSend(sender.sockfd, response.str());
	}

	void	Server::m_execMode( Client &sender, const vector<string> &arg )
	{
		static map< char, int >	userModes = INIT_USERMODES(),
								channelModes = INIT_CHANNELMODES();

		ostringstream	response;

		response << ':' << m_name << ' ';
		if (arg.size() < 3)
			response << ERR_NEEDMOREPARAMS << " * MODE :Not enough parameters";
		else if (!m_isLogged(sender))
			response << ERR_NOTREGISTERED << " * MODE :You have not registered";
	}

	void	Server::m_execDie( Client &sender, const vector<string> &arg )
	{
		ostringstream response;

		response << ':' << m_name << ' ';
		if (!sender.hasMode(UMODE_OPERATOR))
			response << ERR_NOPRIVILEGES << " * DIE :You're not channel operator";
		else
			throw(ShutdownEvent());
		response << "\r\n";
		m_appendToSend(sender.sockfd, response.str());
	}
}
