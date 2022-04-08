/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Leo Suardi <lsuardi@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/15 20:33:28 by lsuardi           #+#    #+#             */
/*   Updated: 2022/03/31 19:49:26 by Leo Suardi       ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "../Client/Client.hpp"
#include "../Channel/Channel.hpp"
#include "../../data/Trie/Trie.hpp"
#include <sys/poll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

namespace irc
{

	class Server
	{
		public:
			Server( std::string = "" );
			~Server( void );

			Server	&open( std::string = "localhost", int = 42 );
			Server	&bind( void );
			Server	&listen( void );
			Server	&poll( int = -1 );
			Server	&handle( void );

		private:
			data::Trie< Channel >	m_channels;		// Channel list
			data::Trie< Client >	m_clients;		// Client list
			std::string				m_password;		// Server password
			int						m_sockfd;		// Server socket FD
			sockaddr_in				m_sockaddr;		// Server socket data
			int						m_sockopt;		// Server socket option value
			pollfd					m_fds[1024];	// FD buffer
			std::size_t				m_nfds;			// Number of active FDs
			bool					m_incoming;		// If true, one of the FDs is readable
	};

}
