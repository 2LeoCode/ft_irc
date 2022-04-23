/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TCPServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Leo Suardi <lsuardi@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/11 18:57:26 by lsuardi           #+#    #+#             */
/*   Updated: 2022/04/23 17:55:23 by Leo Suardi       ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "TCPServer.hpp"

#include <unistd.h>
#include <cstring>
#include <stdexcept>

namespace net {

	TCPServer::TCPServer( void )
	:	m_sockfd(0),
		m_opt(0)
	{
		memset(&m_sockaddr, 0, sizeof(m_sockaddr));
	}

	TCPServer::~TCPServer( void )
	{
		std::vector< pollfd >::iterator	it = m_fds.begin();

		// Close every client / server sockets
		while (it != m_fds.end())
			close(it++->fd);
	}

	TCPServer	&TCPServer::open( int domain, int type, int protocol )
	{
		int	tmp = 0;

		// Open the socket
		m_sockfd = socket(domain, type, protocol);
		if (m_sockfd == -1)
			throw std::runtime_error("socket");

		// Disable IPV6_ONLY flag to allow both IPv4 and IPv6
		if (setsockopt(m_sockfd, SOL_SOCKET, IPV6_V6ONLY, &tmp, 0))
			throw std::runtime_error("setsockopt");

		// Push the socket fd to the pollfd vector used in `poll`
		m_fds.push_back((pollfd){ m_sockfd, POLLIN });
		m_pending.push_back("#server#");
		return *this;
	}

	TCPServer	&TCPServer::bind( short port )
	{
		m_sockaddr.sin6_family = AF_INET6;
		memcpy(&m_sockaddr.sin6_addr, &in6addr_any, sizeof(in6addr_any));
		m_sockaddr.sin6_port = htons(port);

		// Assign address and port to the server socket
		if (::bind(m_sockfd, (sockaddr*)&m_sockaddr, sizeof(m_sockaddr)))
			throw std::runtime_error("bind");
		return *this;
	}

	TCPServer	&TCPServer::listen( int backlog )
	{
		// Make socket able to receive connections and set the
		// maximum length to which the queue of pending connections
		// for sockfd may grow (backlog)
		if (::listen(m_sockfd, backlog))
			throw std::runtime_error("listen");
		return *this;
	}

	unsigned	TCPServer::poll( int timeout )
	{
		typedef std::vector< pollfd >::iterator	iterator;

		// We wait until any event happens (or timeout expired if timeout != -1)
		// in any of the fds inside m_fds vector
		int			ret = ::poll(m_fds.data(), m_fds.size(), timeout);
		iterator	it;

		if (ret < 0)
			throw std::runtime_error("poll");
		else if (ret) {

			// If ret != 0 timeout hasn't expired, so we look through all our fds
			// to check where the event occured, and we return that fd 
			for ( iterator it = m_fds.begin(); it != m_fds.end(); ++it )
				if ( it->revents == POLLIN ) return it - m_fds.begin();
		}

		// We will never get here but we return something to avoid compilation
		// warnings
		return 0;
	}

	TCPClient	TCPServer::accept( void )
	{
		sockaddr_in6	client_addr = { 0 };
		socklen_t		len = sizeof(client_addr);

		// Accept a new connection to the server,
		// store the new returned client socket into fd
		int	fd = ::accept(m_sockfd, (sockaddr*)&client_addr, &len);

		if (fd == -1)
			throw std::runtime_error("accept");
		// Push the new socket inside the pollfd vector used in `poll`
		m_fds.push_back((pollfd){ fd, POLLIN });
		m_pending.push_back("");
		return (TCPClient){ client_addr, len, fd };
	}

	int		TCPServer::recv( unsigned id )
	{
		char		buf[RECV_BUFSIZE + 1];
		std::string	request;
		int			fd;
		int			ret;

		if (id == 0)
			throw std::invalid_argument("recv");
		fd = m_fds.at(id).fd;

		// We do a `recv` loop to read inside the client socket like
		// we would do for a file with `read`
		while ((ret = ::recv(fd, buf, RECV_BUFSIZE, 0)) > 0) {
			buf[ret] = 0;
			request += buf;
		}
		if (ret == 0 && request.empty()) {

			// The client disconnected
			typedef std::vector< pollfd >::const_iterator	iterator;
			std::vector< std::string >::const_iterator	pending_it;

			for (iterator it = m_fds.begin(); it != m_fds.end(); ++it, ++pending_it) {
				if (it->fd == fd) {
					m_pending.erase(pending_it);
					m_fds.erase(it);
					close(fd);
				}
			}
			// Return the fd so we can find it inside our TCPClients
			// to delete it in main
			return fd;
		}
		// if ret == -1 and errno != EWOULDBLOCK an error occured
		if (errno != EWOULDBLOCK)
			throw std::runtime_error("recv");
		return 0;
	}

	TCPServer	&TCPServer::send( unsigned id, std::string request )
	{
		// `send` is the same as `write` but for sockets
		if (::send(m_fds.at(id).fd, request.data(), request.size(), 0) < 0)
			throw std::runtime_error("send");
	}

}
