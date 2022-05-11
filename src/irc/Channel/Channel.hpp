/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Leo Suardi <lsuardi@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/16 00:58:14 by Leo Suardi        #+#    #+#             */
/*   Updated: 2022/05/07 17:09:56 by Leo Suardi       ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include "../../irc.hpp"

#define MODE_INVITEONLY 1
#define MODE_KEYPROTECT (1 << 1)
#define MODE_USERLIMIT (1 << 2)
#define MODE_MODERATE (1 << 3)
#define MODE_NOEXTERNAL (1 << 4)
#define MODE_PRIVATE (1 << 5)
#define MODE_REGISTERED (1 << 6)
#define MODE_SECRET (1 << 7)
#define MODE_TOPICPROT (1 << 8)

namespace irc
{

	class Channel
	{
		public:
			Channel( void );
			~Channel( void );

			string					name;
			string					password;
			map< string, Client >	users;

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
			set< sockaddr_in6 >		m_voiced;
			set< sockaddr_in6 >		m_operators;
			set< sockaddr_in6 >		m_banned;
	};

}
