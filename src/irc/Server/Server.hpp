/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Leo Suardi <lsuardi@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/27 16:24:24 by Leo Suardi        #+#    #+#             */
/*   Updated: 2022/05/02 20:56:49 by Leo Suardi       ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <sys/socket.h>
#include <sys/poll.h>
#include <netinet/in.h>
#include <vector>
#include <string>
#include <map>
#include <queue>

#include "../../irc.hpp"

#define CMD_COUNT 10

namespace irc {

	class Server {
		public:
			Server( void );
			Server( short, string, int = 0, int = 32 );
			~Server();

			Server		&open( short, int = 0, int = 32 );
			void		loop( void );


			struct ShutdownEvent { }; // We throw this object to turn off the server

		private:

			const static std::string cmdTab[10]; //martin ajout

			void		m_send( int, const std::string& );
			//int		send( const std::string&, const std::string& ); Implementer si besoin
			void		m_recv( int );
			void		m_parsePending( void );
			void		m_execCommandQueues( void );

			vector< string >	m_parseCommand(const std::string&); // martin ajout
			int					m_execCommand( const vector< string >&); // martin ajout

			// martin ajout
			int			m_execJoin(const std::vector<std::string> &command);
			int			m_execKick(const std::vector<std::string> &command);
			int			m_execNick(const std::vector<std::string> &command);
			int			m_execSetname(const std::vector<std::string> &command);
			int			m_execPart(const std::vector<std::string> &command);
			int			m_execPass(const std::vector<std::string> &command);
			int			m_execPrivmsg(const std::vector<std::string> &command);
			int			m_execQuit(const std::vector<std::string> &command);
			int			m_execUser(const std::vector<std::string> &command);
			int			m_execOper(const std::vector<std::string> &command);

			void		m_init( int m_sockfd = -1 );

			int										m_sockfd;
			sockaddr_in6							m_addr;
			string									m_password;
			vector< pollfd >						m_pollfd;
			map< int, Client >						m_clients;
			vector< Channel >						m_channels;

			map< int, string >						m_pending;
			map< int, queue< vector < string > > >	m_cmds;

			//vector< string >						m_command; // martin ajout ( gere par une variable non-membre )
			//int									m_commandId; // martin ajout ( gere par une variable non-membre )


			struct {
				bool	v6only,
						reuseaddr;
			}								m_opt;
	};

}
