/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Leo Suardi <lsuardi@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/16 00:53:55 by Leo Suardi        #+#    #+#             */
/*   Updated: 2022/05/31 15:26:32 by Leo Suardi       ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <netinet/in.h>
#include "../../irc.hpp"

#define UMODE_INVISIBLE 1
#define UMODE_GNOTICELOG (1 << 1)
#define UMODE_WALLOPSLOG (1 << 2)
#define UMODE_OPERATOR (1 << 3)

#define MAX_CHANNELS 100

#define INIT_USERMODES( ) ({\
	map< char, int >	um;\
	um['i'] = UMODE_INVISIBLE;\
	um['s'] = UMODE_GNOTICELOG;\
	um['w'] = UMODE_WALLOPSLOG;\
	um['o'] = UMODE_OPERATOR;\
	um;\
})

extern char g_nickCharset[];

namespace irc
{

	class Channel;

	class Client
	{
		public:
			Client( int, sockaddr_in6& );
			~Client( void ); // erase client from all channels

			int					sockfd;
			string				username,
								nickname,
								hostname,
								realname,
								lastpass,
								lastping;

			const sockaddr_in6	&addr( void ) const;

			void				addMode( int );
			void				delMode( int );
			bool				hasMode( int ) const;
			bool				isInvited( const Channel& ) const;
			size_t				channelCount( void ) const;
			void				joinChannel( Channel& );
			void				partChannel( Channel& );

		private:
			sockaddr_in6			m_addr;
			unsigned				m_modes;
			set< const Channel* >	m_curChans;
			set< const Channel* >	m_invites;
	};

	bool	operator <(const Client&, const Client&);
}
