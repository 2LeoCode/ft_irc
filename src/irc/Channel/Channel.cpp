/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Leo Suardi <lsuardi@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/16 01:01:01 by Leo Suardi        #+#    #+#             */
/*   Updated: 2022/05/07 17:33:49 by Leo Suardi       ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"

namespace irc
{

	Channel::Channel( void )
	{
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
		return m_banned.find(c.addr()) != m_banned.end();
	}

	bool	Channel::isVoiced( const Client &c ) const
	{
		return m_voiced.find(c.addr()) != m_voiced.end();
	}

	bool	Channel::isOperator( const Client &c ) const
	{
		try
		{
			m_operators.at(c.nickname.data());
		}
		catch (...)
		{
			return false;
		}
		return true;
	}

}
