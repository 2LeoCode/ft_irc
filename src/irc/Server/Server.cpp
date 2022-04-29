/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Leo Suardi <lsuardi@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/29 15:38:31 by Leo Suardi        #+#    #+#             */
/*   Updated: 2022/04/29 16:59:18 by Leo Suardi       ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include <sys/ioctl.h>
#include <cerrno>
#include <cstring>

namespace irc {

	Server::Server( void )
	{ m_init(); }

	Server::Server( short port, string pass, int protocol, int backlog )
	{
		m_init();
		if (m_sockfd == -1)
			throw std::runtime_error(strerror(errno));

		// Disable IPV6_V6ONLY flag to allow both IPv4 and IPv6
		if (setsockopt(m_sockfd, SOL_SOCKET, IPV6_V6ONLY, &m_opt.v6only, sizeof(bool))
		
		// Snable SO_REUSEADDR flag to be able to launch
		// multiple servers at the same time
		|| setsockopt(m_sockfd, SOL_SOCKET, SO_REUSEADDR, &m_opt.reuseaddr, sizeof(bool))
		
		// Set socket to non-blocking
		|| ioctl(m_sockfd, FIONBIO, &m_opt.fionbio))
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
		|| listen(m_sockfd, 32))
			throw std::runtime_error(strerror(errno));

		// Reserve some space for future clients and
		// push the socket fd to the pollfd array used in `poll`
		m_pollfd.reserve(100);
		m_pollfd.push_back((pollfd){ m_sockfd, POLLIN });

	}

	void	loop( void )
	{
		
	}

	void	Server::m_init( int sockfd = -1 ) {
		m_sockfd = sockfd;
		m_opt.fionbio = true;
		m_opt.reuseaddr = true;
		m_opt.v6only = true;
	}
}
