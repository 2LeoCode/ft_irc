/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Leo Suardi <lsuardi@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/16 00:53:49 by Leo Suardi        #+#    #+#             */
/*   Updated: 2022/05/07 17:02:25 by Leo Suardi       ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include <cstring>
#include <stdexcept>
#include <cerrno>

namespace irc
{

	Client::Client( int fd, sockaddr_in6 &addr )
	:	sockfd( fd ),
		expected( "CAP" ),
		m_addr( addr ),
		m_logged( false ),
		m_modes( 0 )
	{ }

	Client::~Client( void )
	{ }

	void	Client::addMode( int mode )
	{
		m_modes |= mode;
	}

	void	Client::delMode( int mode )
	{
		m_modes &= ~mode;
	}

	bool	Client::hasMode( int mode ) const
	{
		return m_modes & mode;
	}

	bool	Client::isLogged( void ) const
	{
		return m_logged;
	}

	const sockaddr_in6	&Client::addr( void ) const
	{
		return m_addr;
	}

}