/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsuardi <lsuardi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/16 01:01:01 by Leo Suardi        #+#    #+#             */
/*   Updated: 2022/05/16 17:27:03 by lsuardi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"

namespace irc
{

	Channel::Channel( const string &name, const Client &creator )
	:	name(name),
		userLimit( 0 ),
		m_modes( 0 )
	{
		addClient(creator);
		op(creator);
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
		return m_bannedNicknames.find(c.nickname) != m_bannedNicknames.end()
			|| m_bannedHosts.find(c.hostname) != m_bannedHosts.end();
	}

	bool	Channel::isVoiced( const Client &c ) const
	{
		return m_voiced.find(&c) != m_voiced.end();
	}

	bool	Channel::isOperator( const Client &c ) const
	{
		return m_operators.find(&c) != m_operators.end();
	}

	void	Channel::addClient( const Client &c )
	{
		users.insert(&c);
	}

	void	Channel::delClient( const Client &c )
	{
		users.erase(&c);
	}

	void	Channel::op( const Client &c )
	{
		if (users.find(&c) == users.end())
			throw ClientNotInChannel();
		m_operators.insert(&c);
	}

	void	Channel::deop( const Client &c )
	{
		if (users.find(&c) == users.end())
			throw ClientNotInChannel();
		m_operators.erase(&c);
	}

	void	Channel::banNickname( const string &nick )
	{
		try
		{
			m_bannedNicknames.insert(nick);
		}
		catch (...)
		{ }
	}

	void	Channel::unbanNickname( const string &nick )
	{
		m_bannedNicknames.erase(nick);
	}

	void	Channel::banHostname( const string &host )
	{
		try
		{
			m_bannedHosts.insert(host);
		}
		catch (...)
		{ }
	}

	void	Channel::unbanNickname( const string &nick )
	{
		m_bannedNicknames.erase(nick);
	}

	void	Channel::unbanHostname( const string &host )
	{
		m_bannedHosts.erase(host);
	}

	void	Channel::kick( const Client &c )
	{
		m_operators.erase(&c);
		m_voiced.erase(&c);
		users.erase(&c);
	}

	void	Channel::voice( const Client &c )
	{
		if (users.find(&c) == users.end())
			throw ClientNotInChannel();
		m_voiced.insert(&c);
	}

	void	Channel::unvoice( const Client &c )
	{
		if (users.find(&c) == users.end())
			throw ClientNotInChannel();
		m_voiced.erase(&c);
	}

}
