/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsuardi <lsuardi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/16 00:58:14 by Leo Suardi        #+#    #+#             */
/*   Updated: 2022/05/14 14:56:51 by lsuardi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include "../../irc.hpp"

#define CMODE_PRIVATE 1
#define CMODE_SECRET (1 << 1)
#define CMODE_INVITEONLY (1 << 2)
#define CMODE_TOPICPROTECT (1 << 3)
#define CMODE_NOOUTSIDEMSG (1 << 4)
#define CMODE_MODERATED (1 << 5)

#define INIT_CHANNELMODES( ) ({\
	map< char, int >	cm;\
	cm['p'] = CMODE_PRIVATE;\
	cm['s'] = CMODE_SECRET;\
	cm['i'] = CMODE_INVITEONLY;\
	cm['t'] = CMODE_TOPICPROTECT;\
	cm['n'] = CMODE_NOOUTSIDEMSG;\
	cm['m'] = CMODE_MODERATED;\
	cm;\
})

namespace irc
{
	class Client;

	class Channel
	{
		public:
			Channel( void );
			~Channel( void ); // erase channel from all clients

			string					name;
			string					password;
			map< string, Client* >	users;

			void	addMode( int );
			void	delMode( int );
			bool	hasMode( int ) const;

			bool	isBanned( const Client& ) const;
			bool	isVoiced( const Client& ) const;
			bool	isOperator( const Client& ) const;

			void	addClient( Client& );
			void	delClient( Client& );

			void	opClient( const Client& );
			void	deopClient( const Client& );

			void	banClient( const Client& );
			void	unbanClient( const Client& );

			void	voiceClient( const Client& );
			void	unvoiceClient( const Client& );

			void	setUserLimit( size_t );
			void	getUserLimit( size_t );

		private:
			int						m_modes;
			size_t					m_userLimit;
			set< in6_addr >			m_voiced;
			set< in6_addr >			m_operators;
			set< in6_addr >			m_banned;
	};

}
