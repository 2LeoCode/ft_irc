/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Leo Suardi <lsuardi@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/29 15:38:31 by Leo Suardi        #+#    #+#             */
/*   Updated: 2022/05/07 17:04:26 by Leo Suardi       ###   ########.fr       */
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
	{
		m_init();
	}

	Server::~Server( void )
	{
		typedef vector< pollfd >::const_iterator	iter;

		for (iter it = m_pollfd.begin(); it != m_pollfd.end(); ++it)
			close(it->fd);
	}

	

	Server::Server( short port, string pass, int protocol, int backlog )
	:	m_sockfd(0)
	{
		open(port, pass, protocol, backlog);
	}

	Server	&Server::open( short port, string pass, int protocol, int backlog )
	{
		if (m_sockfd)
			throw std::runtime_error("Server already opened");
		m_init(socket(AF_INET6, SOCK_STREAM, protocol));
		if (m_sockfd == -1)
			throw std::runtime_error(strerror(errno));

		// Disable IPV6_V6ONLY flag to allow both IPv4 and IPv6
		if (setsockopt(m_sockfd, SOL_SOCKET, IPV6_V6ONLY, &m_opt.v6only, sizeof(bool))
		
		// Snable SO_REUSEADDR flag to be able to launch
		// multiple servers at the same time
		|| setsockopt(m_sockfd, SOL_SOCKET, SO_REUSEADDR, &m_opt.reuseaddr, sizeof(bool)))
			throw std::runtime_error(strerror(errno));

		const int flags = fcntl(m_sockfd, F_GETFL);
		if (fcntl(m_sockfd, F_SETFL, flags | O_NONBLOCK))
			throw std::runtime_error(strerror(errno));

		// Fill up the sockaddr struct for the server
		m_addr.sin6_family = AF_INET6;
		memcpy(&m_addr.sin6_addr, &in6addr_any, sizeof(in6addr_any));
		m_addr.sin6_port = htons(port);

		// Bind it to the socket
		if (bind(m_sockfd, reinterpret_cast<sockaddr*>(&m_addr), sizeof(m_addr))

		// And make socket able to receive connections and set the
		// maximum length to which the queue of pending connections
		// for sockfd may grow (backlog)
		|| listen(m_sockfd, backlog))
			throw std::runtime_error(strerror(errno));

		// Reserve some space for future clients and
		// push the socket fd to the pollfd array used in `poll`
		m_pollfd.reserve(100);
		m_pollfd.push_back((pollfd){ m_sockfd, POLLIN });
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
				throw std::runtime_error(strerror(errno));

			// Check which fd is rceadable by cheking pollfd::revents 
			// (if revents == POLLIN the fd is readable)

			// If we get POLLIN on the server socket, it means that a new
			// client is trying to connect
			if (m_pollfd[0].revents == POLLIN)
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
					m_recv(it->fd);
			}

			// Now check if we have full commands pending,
			// and if so, execute them
			m_parsePending();
			m_execCommandQueues();
		}
	}

	void	Server::m_recv( int fd )
	{
		static char	buf[BUFFER_SIZE + 1] = { 0 };
		int			ret;

		do
		{
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
					throw std::runtime_error(strerror(errno));
			}

			// If recv returned 0 it means that the client closed connection
			else if (!ret)
				m_kickClient(m_clients[fd]);

			// Otherwise we read something, so we append it
			// to the pending buffer
			else
			{
				buf[ret] = 0;
				m_pending[fd] += buf;
			}
		}
		while (ret > 0); // Loop until there is nothing to read
	}

	void	Server::m_send( int fd, const string &request )
	{
		if (send(fd, request.data(), request.size(), 0))
			throw std::runtime_error(strerror(errno));
	}

	void	Server::m_init( int sockfd = -1 )
	{
		static bool	dummy = m_init_exec();
		m_sockfd = sockfd;
		m_opt.reuseaddr = true;
		m_opt.v6only = true;
	}

	bool	Server::m_init_exec( void )
	{
		m_execs.insert("CAP", m_execCap)
			.insert("PASS", m_execPass)
			.insert("NICK", m_execNick)
			.insert("USER", m_execUser)
			.insert("OPER", m_execOper)
			.insert("DIE", m_execDie)
			.insert("GLOBOPS", m_execGlobops)
			.insert("HELP", m_execHelp)
			.insert("IMPORTMOTD", m_execImportmotd)
			.insert("INFO", m_execInfo)
			.insert("INVITE", m_execInvite)
			.insert("ISBANNED", m_execIsbanned)
			.insert("ISON", m_execIson)
			.insert("KILL", m_execKill)
			.insert("KILLBAN", m_execKillban)
			.insert("UNBAN", m_execUnban)
			.insert("SHUN", m_execShun)
			.insert("LIST", m_execList)
			.insert("MODE", m_execMode)
			.insert("JOIN", m_execJoin)
			.insert("KICK", m_execKick)
			.insert("SETNAME", m_execSetname)
			.insert("PART", m_execPart)
			.insert("MSG", m_execMsg)
			.insert("PRIVMSG", m_execMsg)
			.insert("QUIT", m_execQuit)
			.insert("BYE", m_execQuit)
			.insert("ME", m_execMe)
			.insert("NOTICE", m_execNotice)
			.insert("NAMES", m_execNames)
			.insert("TIME", m_execTime)
			.insert("TOPIC", m_execTopic)
			.insert("USERHOST", m_execUserhost)
			.insert("VERSION", m_execVersion)
			.insert("WALL", m_execWall)
			.insert("WALLOPS", m_execWallops)
			.insert("WHO", m_execWho)
			.insert("RESTART", m_execRestart);
		return false;
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
		int			id = -1;
		const char	*cmd_name = command.begin()->data();

		if (!sender.isLogged() && sender.expected != cmd_name)
			m_kickClient(sender);
		try
		{
			(this->*m_execs.at(cmd_name))(sender, command);
		}
		catch ( const std::exception &e )
		{
			cout << "unknown command" << endl;
		}
		return (-1);
	}

	void	Server::m_parsePending( void )
	{
		typedef map< int, string >::iterator					InputIt;
		typedef map< int, queue< vector< string > > >::iterator	OutputIt;

		// Call m_parseCommand on each available command in the pending buffer
		// and push the newly created command into the execute queue
		for (InputIt it = m_pending.begin(); it != m_pending.end(); ++it)
		{
			size_t	found;

			while ((found = it->second.find("\r\n")) != string::npos)
			{
				m_cmds[it->first].push(m_parseCommand(it->second.substr(0, found + 2)));

				// Erase the command from the pending buffer
				it->second.erase(0, found + 2);
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
				m_execCommand(m_clients[it->first], it->second.front());
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
		m_pending.erase(it->fd);
		m_cmds.erase(it->fd);
		m_clients.erase(it->fd);
		m_pollfd.erase(it);
	}

	int	Server::m_execCap( Client &sender, const vector<string> &arg )
	{
		ostringstream	response;

		if (sender.isLogged())
		{
			response << ':' << m_name << ' ' << ERR_ALREADYREGISTRED
				<< " * :You may not reregister";
		}
		else if (!sender.logLevel())
		{
			if (arg.size() < 2)
				response << ':' << m_name << ' ' << ERR_NEEDMOREPARAMS
					<< " * CAP :Not enough parameters";
			else if (arg[2] != "LS")
				response << ':' << m_name << ' ' << ERR_INVALIDCAPCMD
					<< " * CAP :Invalid sub-command";
			sender.
		}
		else if (sender.log)
	}

}
