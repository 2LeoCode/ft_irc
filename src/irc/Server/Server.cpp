/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Leo Suardi <lsuardi@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/29 15:38:31 by Leo Suardi        #+#    #+#             */
/*   Updated: 2022/05/31 17:17:23 by Leo Suardi       ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include <sys/ioctl.h>
#include <cerrno>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>

#define BUFFER_SIZE 2048

#define MODE_APPLY( true_action, false_action ) \
	do\
		if (add)\
			true_action ;\
		else\
			false_action ;\
	while (0)

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
			throw runtime_error("Server already opened");

		if ((m_sockfd = socket(AF_INET6, SOCK_STREAM, protocol)) == -1)
			throw runtime_error(string("socket: ") + strerror(errno));

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
			.insert("NAMES", &Server::m_execNames);
			//.insert("PONG", &Server::m_execPong);
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
			//.insert("PART", &Server::m_execPart)
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
		typedef vector< pollfd >::iterator	iter;

		int ret, client_fd;

		while (true) {
			// Poll will block until any of the sockets is readable
			ret = poll(m_pollfd.data(), m_pollfd.size(), -1);

			if (ret < 0)
				throw runtime_error(string("poll: ") + strerror(errno));

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
		catch ( const exception &e )
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
			return ;
		}
		m_appendToSend(sender.sockfd, response.str());
	}

	void	Server::m_execNick( Client &sender, const vector<string> &arg )
	{
		ostringstream	response;

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
		ostringstream	newHostName("@u");
		newHostName << currentHostID++;
		user.hostname = newHostName.str();
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
			return ;
		}
		m_appendToSend(sender.sockfd, response.str());
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

		if (arg.size() < 3)
			response << m_prefix() << ERR_NEEDMOREPARAMS << " * MODE :Not enough parameters" << m_endl();
		else if (!m_isLogged(sender))
			response << m_prefix() << ERR_NOTREGISTERED << " * MODE :You have not registered" << m_endl();
		else
		{
			string::const_iterator	it = arg[2].begin();

			if (*it == '-' || *it == '+')
				add = (*it++ == '+');
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
				if (!sender.hasMode(UMODE_OPERATOR))
				{
					if (chan->users.find(&sender) == chan->users.end())
					{
						response << m_prefix() << ERR_NOTONCHANNEL << ' ' << arg[1] << " :You're not on that channel" << m_endl();
						goto END;
					}
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
								break ;
							case 'l':
								MODE_APPLY(chan->userLimit = limit, chan->userLimit = 0);
								break ;
							case 'b':
								if (banByHost)
									MODE_APPLY(chan->banHostname(user->hostname), chan->unbanHostname(user->hostname));
								else
									MODE_APPLY(chan->banNickname(user->nickname), chan->unbanNickname(user->nickname));
								break ;
							case 'v':
								MODE_APPLY(chan->voice(*user), chan->unvoice(*user));
								break ;
							case 'k':
								MODE_APPLY(chan->password = *key, chan->password.clear());
								break ;
							default:
								try
								{
									MODE_APPLY(chan->addMode(channelModes.at(*it)), chan->delMode(channelModes.at(*it)));
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
				while (it != arg[2].end())
				{
					if (!sender.hasMode(UMODE_OPERATOR))
					{
						if (*it == 'o')
							response << m_prefix() << ERR_NOPRIVILEGES << " * MODE :Permission Denied- You're not an IRC operator" << m_endl();
						else if (arg[1] != sender.username)
							response << m_prefix() << ERR_USERSDONTMATCH << " * MODE :Cant change mode for other users" << m_endl();
					}
					else
					{
						try
						{
							MODE_APPLY(user->addMode(userModes.at(*it)), user->delMode(userModes.at(*it)));
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

		END:
		m_appendToSend(sender.sockfd, response.str());
	}

	void	Server::m_execDie( Client &sender, const vector<string> &arg )
	{
		ostringstream response;

		(void)arg;
		if (!m_isLogged(sender))
			response << m_prefix() << ERR_NOTREGISTERED << " * DIE :You have not registeredr" << m_endl();
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
		typedef vector<string>::const_iterator Iter;
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
				Channel				*cur = chansToJoin.front();
				chansToJoin.pop();

				if (sender.channelCount() == MAX_CHANNELS)
				{
					response << ERR_TOOMANYCHANNELS << cur->name << " :You have joined to many channels" << m_endl();
					continue ;
				}
				if (cur->hasMode(CMODE_INVITEONLY) && !sender.isInvited(*cur))
				{
					response << m_prefix() << ERR_INVITEONLYCHAN << cur->name << " :Cannot join channel (+i)" << m_endl();
					continue ;
				}
				if (cur->isBanned(sender))
				{
					response << m_prefix() << ERR_BANNEDFROMCHAN << cur->name << " :Cannot join channel (+b)" << m_endl();
					continue ;
				}
				if (!cur->password.empty())
				{
					if (currentKey == keys.end())
						goto BAD_KEY;
					if (*currentKey != cur->password)
					{
						BAD_KEY:
						response << m_prefix() << ERR_BADCHANNELKEY << cur->name << " :Cannot join channel (+k)" << m_endl();
						continue ;
					}
				}
				if (cur->userLimit != 0 && cur->users.size() == cur->userLimit)
				{
					response << m_prefix() << ERR_CHANNELISFULL << cur->name << " :Cannot join channel (+l)" << m_endl();
					continue ;
				}
				sender.joinChannel(*cur);
				if (cur->users.size() == 1)
					cur->op(sender);
				response << m_prefix() << "JOIN :" << cur->name << m_endl();
				response << m_prefix() << RPL_TOPIC << ' ' << cur->name << " :" << cur->topic << m_endl();
				m_execNames(sender, m_make_args(2, "NAMES", cur->name.data()));
			}
		}
		m_appendToSend(sender.sockfd, response.str());
	}

	// Reste a coder l'envoi du message et a checker la validite des clients avant de les push sur les vecteurs de targets.
	void	Server::m_execPrivmsg(Client &sender, const vector<string> &arg)
	{
		ostringstream response;
		vector< const Client* > privTargets, chanTargets;
		

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
			typedef vector<string>::iterator iter;
			vector<string> receivers = split(arg[1], ',');
			for (iter it = receivers.begin(); it != receivers.end(); it++)
			{
				if (*it->data() == '#' || *it->data() == '&')
				{
					//push every client of channel to chanTargets
					try
					{
						typedef set<const irc::Client*>::iterator cliIter;
						Channel *ptr = &m_channels.at(*it);
						if (sender.hasMode(UMODE_OPERATOR) || ptr->canSpeak(sender))
						{
							for (cliIter cliIt = ptr->users.begin(); cliIt != ptr->users.end(); cliIt++)
							{
								if ((*cliIt)->sockfd != sender.sockfd)
									chanTargets.push_back(*cliIt);
							}
						}
					}
					catch (...)
					{
						response << m_prefix() << ERR_NOSUCHNICK << *it << " :No such nick/channel" << m_endl();
					}
				}
				else
				{
					// ===> /!\/!\ NEED TO ADD A CHECK BEFORE PUSHING /!\/!\ <===
					privTargets.push_back(&m_findClient(*it));
					//push client to privTargets
				}
			}
			//loop through target
			typedef vector<const irc::Client*>::iterator privIter;
			for (privIter privIt = privTargets.begin(); privIt != privTargets.end(); privIt++)
			{
				// send message
			}
			typedef vector<const irc::Client*>::iterator chanIter;
			for (chanIter chanIt = chanTargets.begin(); chanIt != chanTargets.end(); chanIt++)
			{
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
				for (Iter it = c->users.begin(); it != c->users.end() && response << ' '; ++it)
				{
					if (c->isOperator(**it))
						response << '@';
					response << (*it)->nickname;
				}
				response << m_endl() << m_prefix() << RPL_ENDOFNAMES << ' ' << *curChannel << " :End of /NAMES list" << m_endl();
				++curChannel;
			}
		}
		m_appendToSend(sender.sockfd, response.str());
	}

}
