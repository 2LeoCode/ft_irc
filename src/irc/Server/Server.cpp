/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Leo Suardi <lsuardi@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/15 20:47:58 by lsuardi           #+#    #+#             */
/*   Updated: 2022/04/01 16:37:16 by Leo Suardi       ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include <cstring>
#include <unistd.h>

namespace irc
{

	Server::Server( std::string password )
	:	m_password(password), m_sockfd(0), m_sockopt(1), m_nfds(0)
	{ }
	
	Server::~Server( void )
	{
		for (size_t i = 0; i < m_nfds; i++)
  		{
			if (m_fds[i].fd >= 0)
				close(m_fds[i].fd);
		}
	}

	Server	&Server::open( std::string address, int port )
	{
		if (!m_nfds)
		m_sockfd = socket(AF_INET, SOCK_STREAM, 0);
		if (m_sockfd == -1)
			throw socket_failure();
		std::memset(&m_sockaddr, 0, sizeof(m_sockaddr));
		m_fds[0]
	}

}
