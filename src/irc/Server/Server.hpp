/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Leo Suardi <lsuardi@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/27 16:24:24 by Leo Suardi        #+#    #+#             */
/*   Updated: 2022/04/29 16:58:56 by Leo Suardi       ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <sys/socket.h>
#include <sys/poll.h>
#include <netinet/in.h>
#include <vector>
#include <string>
#include <map>

#include "../data.hpp"
#include "../../irc.hpp"

using std::string;
using std::vector;
using std::map;


namespace irc {

	class Server {
		public:
			Server();
			Server( short, string, int = 0, int = 32 );
			~Server();

			Server		&open( short, int = 0, int = 32 );
			int			run( void );

			void		loop( void );
			int			send( int, const std::string& );
			int			send( const std::string&, const std::string& );
			int			recv( int );


		private:
			void		m_init( int m_sockfd = -1 );

			int								m_sockfd;
			sockaddr_in6					m_addr;
			std::string						m_password;
			std::vector< pollfd >			m_pollfd;
			std::map< int, std::string >	m_pending;
			std::map< int, Client* >		m_clients_by_fd;
			data::Trie< Channel >			m_channels;
			data::Trie< Client >			m_clients;
			struct {
				bool v6only, reuseaddr, fionbio;
			}								m_opt;
	};

}
