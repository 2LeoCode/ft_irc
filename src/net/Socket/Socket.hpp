/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsuardi <lsuardi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/11 16:52:52 by lsuardi           #+#    #+#             */
/*   Updated: 2022/04/12 14:00:08 by lsuardi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <sys/socket.h>
#include <sys/poll.h>
#include <netinet/in.h>
#include <vector>
#include <string>

namespace net {

	class Socket {
		public:
			Socket( void );
			~Socket( );

			Socket	&open( int = AF_INET6, int = SOCK_STREAM, int = 0 );
			Socket	&bind( short );
			Socket	&listen( int = 32 );
			Socket	&poll( int = -1 );
			Socket	&accept( void );
			Socket	&recv( void );
			Socket	&handle( void );

		private:
			int							m_sockfd;
			int							m_opt;
			sockaddr_in6				m_sockaddr;
			std::vector< pollfd >		m_fds;
			std::vector< std::string >	m_pending;
			size_t						m_nfds;
	};

}
