/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Leo Suardi <lsuardi@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/16 01:01:01 by Leo Suardi        #+#    #+#             */
/*   Updated: 2022/05/15 17:02:23 by Leo Suardi       ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"

namespace irc
{

	Channel::Channel( const string &name, Client *creator )
	:	name(name),
		m_modes( 0 ),
		m_userLimit( 0 )
	{
		m_operators.insert(creator->nickname);
	}


	Channel::~Channel( void )
	{
	}

	void	Channel::addMode( int mode )
	{
		m_modes |= mode;
	}

	void	Channel::delMode( int mode )
	{
		m_modes &= ~mode;
	}

	bool	Channel::hasMode( int mode ) const
	{
		return m_modes & mode;
	}

	bool	Channel::isBanned( const Client &c ) const
	{
		return m_banned.find(c.addr().sin6_addr) != m_banned.end();
	}

	bool	Channel::isVoiced( const Client &c ) const
	{
		return m_voiced.find(c.addr().sin6_addr) != m_voiced.end();
	}

	bool	Channel::isOperator( const Client &c ) const
	{
		return m_operators.find(c.addr().sin6_addr) != m_operators.end();
	}

}
