/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Leo Suardi <lsuardi@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/16 00:53:49 by Leo Suardi        #+#    #+#             */
/*   Updated: 2022/05/20 00:20:13 by Leo Suardi       ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include <cstring>
#include <stdexcept>
#include <cerrno>

char	g_nickCharset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ123456789-_[]{}\\`|";

namespace irc
{

	Client::Client( int fd, sockaddr_in6 &addr )
	:	sockfd( fd ),
		m_addr( addr ),
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

	const sockaddr_in6	&Client::addr( void ) const
	{
		return m_addr;
	}

	bool	Client::isInvited( const Channel &c ) const 
	{
		return m_invites.find(&c) != m_invites.end();
	}

	bool	Client::channelCount( void ) const
	{
		return m_curChans.size();
	}

	void	Client::joinChannel( Channel &c )
	{
		m_curChans.insert(&c);
		c.addClient(*this);
	}

	void	Client::partChannel( Channel &c )
	{
		m_curChans.erase(&c);
		c.delClient(*this);
	}

	bool	operator <( const Client &lhs, const Client &rhs )
	{
		return lhs.sockfd < rhs.sockfd;
	}

}
