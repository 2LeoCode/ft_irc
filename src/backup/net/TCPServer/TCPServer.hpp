/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TCPServer.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Leo Suardi <lsuardi@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/11 16:52:52 by lsuardi           #+#    #+#             */
/*   Updated: 2022/04/23 17:57:01 by Leo Suardi       ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <sys/socket.h>
#include <sys/poll.h>
#include <netinet/in.h>
#include <vector>
#include <string>
#include "../TCPClient/TCPClient.hpp"

#define RECV_BUFSIZE 1024

namespace net {

	class TCPServer {
		public:
			TCPServer( void );
			~TCPServer( );

			TCPServer	&open( int = AF_INET6, int = SOCK_STREAM, int = 0 );
			TCPServer	&bind( short );
			TCPServer	&listen( int = 32 );
			unsigned	poll( int = -1 );
			TCPClient	accept( void );
			int			recv( unsigned );
			TCPServer	&send( unsigned, std::string );

		private:
			int							m_sockfd;
			int							m_opt;
			sockaddr_in6				m_sockaddr;
			std::vector< pollfd >		m_fds;
			std::vector< std::string >	m_pending;
	};

}
