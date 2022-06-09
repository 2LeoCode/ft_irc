/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Leo Suardi <lsuardi@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/16 00:58:14 by Leo Suardi        #+#    #+#             */
/*   Updated: 2022/06/09 15:25:54 by Leo Suardi       ###   ########.fr       */
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
			Channel( const string& );
			Channel( const Channel& );
			~Channel( void ); // erase channel from all clients

			string					name;
			string					topic;
			string					password;
			size_t					userLimit;
			set< const Client* >	users;

			void	addMode( int );
			void	delMode( int );
			bool	hasMode( int ) const;

			bool	empty( void ) const;
			bool	hasClient( const Client& ) const;
			bool	isBanned( const Client& ) const;
			bool	isVoiced( const Client& ) const;
			bool	isOperator( const Client& ) const;
			bool	canSpeak( const Client& ) const;

			void	addClient( const Client& );
			void	delClient( const Client& );

			void	op( const Client& );
			void	deop( const Client& );

			void	banNickname( const string& );
			void	unbanNickname( const string& );

			void	banHostname( const string& );
			void	unbanHostname( const string& );

			void	kick( const Client& );

			void	voice( const Client& );
			void	unvoice( const Client& );

			string	getModes( void ) const;

			EXCEPTION( ClientNotInChannel, "Client is not in channel" );

		private:
			int						m_modes;
			set< const Client* >	m_voiced;
			set< const Client* >	m_operators;
			set< string >			m_bannedHosts;
			set< string >			m_bannedNicknames;
	};

}
