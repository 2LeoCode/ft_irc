/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsuardi <lsuardi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/15 20:47:58 by lsuardi           #+#    #+#             */
/*   Updated: 2022/04/08 15:31:55 by lsuardi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include <cstring>
#include <unistd.h>
#include "../exceptions.hpp"

namespace irc
{

	Server::Server( std::string password )
	:	m_password(password),
		m_sockfd(0),
		m_sockopt(1),
		m_nfds(0),
		m_sockaddr()
	{ }
	
	Server::~Server( void )
	{
		m_clear();
	}

	Server	&Server::open( void )
	{
		if (m_sockfd)
			m_clear();
		m_sockfd = socket(AF_INET, SOCK_STREAM, 0);
		if (m_sockfd == -1)
			throw socket_failure();
		setsockopt(m_sockfd, SOL_SOCKET, SO_REUSEADDR, &m_sockopt, sizeof(m_sockopt));
		return *this;
	}

	Server	&Server::bind( std::string addr, int port )
	{
		if (m_nfds)
			throw socket_already_bound();
		if (!m_sockfd)
			throw uninitialized_socket();
		m_sockaddr.sin_family = AF_INET;
		memcpy(&m_sockaddr.sin_addr, addr.data(), addr.length());
		m_sockaddr.sin_port = htons(port);
		::bind(m_sockfd, (sockaddr*)&m_sockaddr, sizeof(sockaddr));
		m_fds->fd = m_sockfd;
		m_fds->events = POLLIN;
		return *this;
	}

	Server	&Server::listen( int backlog )
	{
		if (!m_sockfd)
			throw uninitialized_socket();
		if (!m_nfds)
		::listen(m_sockfd)
	}

	// PRIVATE METHODS

	void	Server::m_clear( void )
	{
		for (size_t i = 0; i < m_nfds; ++i)
  		{
			if (m_fds[i].fd >= 0)
				close(m_fds[i].fd);
		}
		m_nfds = 0;
	}
}
