/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Leo Suardi <lsuardi@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/16 00:53:49 by Leo Suardi        #+#    #+#             */
/*   Updated: 2022/04/29 18:48:08 by Leo Suardi       ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include <cstring>
#include <stdexcept>
#include <cerrno>

namespace irc
{

	Client::Client( int sockfd, sockaddr_in6 &addr )
	:	m_sockfd(sockfd),
		m_addr(addr)
	{ }

	Client::~Client( void )
	{
		delete m_username;
		delete m_nickname;
		delete m_realname;
	}

}