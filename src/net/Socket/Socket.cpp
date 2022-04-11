/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsuardi <lsuardi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/11 18:57:26 by lsuardi           #+#    #+#             */
/*   Updated: 2022/04/11 20:11:20 by lsuardi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Socket.hpp"

#include <unistd.h>
#include <cstring>
#include <stdexcept>

namespace net {

	Socket::Socket( void )
	:	m_sockfd(0),
		m_opt(0),
		m_nfds(0)
	{
		memset(&m_sockaddr, 0, sizeof(m_sockaddr));
	}

	Socket::~Socket( void )
	{
		std::vector< pollfd >::iterator	it = m_fds.begin();

		while (it != m_fds.end())
			close(it++->fd);
	}

	Socket	&Socket::open( int domain, int type, int protocol )
	{
		int	tmp = 0;

		m_sockfd = socket(domain, type, protocol);
		if (m_sockfd == -1)
			throw std::runtime_error("socket");
		if (setsockopt(m_sockfd, SOL_SOCKET, IPV6_V6ONLY, &tmp, 0))
			throw std::runtime_error("setsockopt");
		m_fds[0].fd = m_sockfd;
		m_fds[0].events = POLLIN;
		m_nfds = 1;
		return *this;
	}

	Socket	&Socket::bind( short port )
	{
		m_sockaddr.sin6_family = AF_INET6;
		memcpy(&m_sockaddr.sin6_addr, &in6addr_any, sizeof(in6addr_any));
		m_sockaddr.sin6_port = htons(port);
		if (::bind(m_sockfd, (sockaddr*)&m_sockaddr, sizeof(m_sockaddr)))
			throw std::runtime_error("bind");
		return *this;
	}

	Socket	&Socket::listen( int backlog )
	{
		if (::listen(m_sockfd, backlog))
			throw std::runtime_error("listen");
		return *this;
	}

	Socket	&Socket::poll( int timeout )
	{
		if (::poll(m_fds.data(), m_nfds, timeout))
			throw std::runtime_error("poll");
		return *this;
	}

	Socket	&Socket::accept( void )
	{
		//sockaddr_in6	client_addr;
		//socklen_t		client_addr_len = sizeof(client_addr);
		int	fd = ::accept(m_sockfd, NULL, NULL);

		if (fd == -1)
			throw std::runtime_error("accept");
		m_fds.push_back((pollfd){ fd, POLLIN });
		++m_nfds;
		return *this;
	}

	Socket	&Socket::recv( std::string &s )
	{

	}

	Socket	&Socket::send( std::string s )
	{
		
	}

}
