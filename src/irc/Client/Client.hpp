/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Leo Suardi <lsuardi@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/16 00:53:55 by Leo Suardi        #+#    #+#             */
/*   Updated: 2022/05/07 17:01:54 by Leo Suardi       ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <netinet/in.h>
#include "../../irc.hpp"

#define MODE_ADMIN 1
#define MODE_CLIENTLOG (1 << 1)
#define MODE_GNOTICELOG (1 << 2)
#define MODE_HELPER (1 << 3)
#define MODE_INVISIBLE (1 << 4)
#define MODE_OPERATOR (1 << 5)
#define MODE_SERVERLOG (1 << 6)
#define MODE_WALLOPSLOG (1 << 7)

namespace irc
{

	class Client
	{
		public:
			Client( int, sockaddr_in6& );
			~Client( void );

			int				sockfd;
			string			username,
							nickname,
							hostname,
							realname,
							expected;

			const sockaddr_in6	&addr( void ) const;

			void				addMode( int );
			void				delMode( int );
			bool				hasMode( int ) const;

			bool				isLogged( void ) const;

		private:
			sockaddr_in6		m_addr;
			bool				m_logged;
			unsigned			m_modes;
			string				m_ping, m_pong;
			Trie< Channel* >	m_curChans;
			Trie< Channel* >	m_invites;
	};

}
