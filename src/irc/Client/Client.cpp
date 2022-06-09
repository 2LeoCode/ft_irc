/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Leo Suardi <lsuardi@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/16 00:53:49 by Leo Suardi        #+#    #+#             */
/*   Updated: 2022/06/09 15:34:46 by Leo Suardi       ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include <cstring>
#include <stdexcept>
#include <cerrno>
#include <unistd.h>

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

	size_t	Client::channelCount( void ) const
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
		c.kick(*this);
	}

	void	Client::exitAllChans( map< string, Channel >& globChans )
	{
		typedef set< Channel* >::iterator iter;
	
		for (iter it = m_curChans.begin(); it != m_curChans.end(); ++it)
		{
			(*it)->kick(*this);
			if ((*it)->empty())
				globChans.erase((*it)->name);
		}
		m_curChans.clear();
	}

	bool	operator <( const Client &lhs, const Client &rhs )
	{
		return lhs.sockfd < rhs.sockfd;
	}

	string Client::makePrefix( void ) const
	{
		return ":" + nickname + "!~" + username + hostname + " ";
	}

	string Client::getModes( void ) const
	{
		string ret = "+";

		if (hasMode(UMODE_INVISIBLE))
			ret += 'i';
		if (hasMode(UMODE_GNOTICELOG))
			ret += 's';
		if (hasMode(UMODE_WALLOPSLOG))
			ret += 'w';
		if (hasMode(UMODE_OPERATOR))
			ret += 'o';
		if (ret.length() == 1)
			ret.clear();
		return ret;
	}

}
