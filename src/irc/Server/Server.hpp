/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Leo Suardi <lsuardi@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/27 16:24:24 by Leo Suardi        #+#    #+#             */
/*   Updated: 2022/05/07 18:39:36 by Leo Suardi       ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <sys/socket.h>
#include <sys/poll.h>
#include <netinet/in.h>

#include "../../irc.hpp"

#define CMD_COUNT 10

namespace irc {

	class Server {
		public:
			Server( void );
			Server( short, string, int = 0, int = 32 );
			~Server();

			Server		&open( short, string, int = 0, int = 32 );
			void		loop( void );


			struct ShutdownEvent { }; // We throw this object to turn off the server

		private:

			void		m_send( int, const std::string& );
			//int		send( const std::string&, const std::string& ); Implementer si besoin
			void		m_recv( int );
			void		m_parsePending( void );
			void		m_execCommandQueues( void );
			void		m_kickClient( Client& );

			vector< string >	m_parseCommand( const std::string& ); // martin ajout
			int					m_execCommand( Client&, const vector< string >& ); // martin ajout

			// martin ajout
			int			m_execCap( Client&, const vector<string>& );
			int			m_execPass( Client&, const vector<string>& );
			int			m_execNick( Client&, const vector<string>& );
			int			m_execUser( Client&, const vector<string>& );
			int			m_execOper( Client&, const vector<string>& );
			int			m_execDie( Client&, const vector<string>& );
			int			m_execGlobops( Client&, const vector<string>& );
			int			m_execHelp( Client&, const std::vector<string>& );
			int			m_execImportmotd( Client&, const vector<string>& );
			int			m_execInfo( Client&, const vector<string>& );
			int			m_execInvite( Client&, const vector<string>& );
			int			m_execIsbanned( Client&, const vector<string>& );
			int			m_execIson( Client&, const vector<string>& );
			int			m_execKill( Client&, const vector<string>& );
			int			m_execKillban( Client&, const vector<string>& );
			int			m_execUnban( Client&, const vector<string>& );
			int			m_execShun( Client&, const vector<string>& );
			int			m_execList( Client&, const vector<string>& );
			int			m_execMode( Client&, const vector<string>& );
			int			m_execJoin( Client&, const vector<string>& );
			int			m_execKick( Client&, const vector<string>& );
			int			m_execSetname( Client&, const vector<string>& );
			int			m_execPart( Client&, const vector<string>& );
			int			m_execQuit( Client&, const vector<string>& );
			int			m_execMe( Client&, const vector<string>& );
			int			m_execMsg( Client&, const vector<string>& );
			int			m_execNotice( Client&, const vector<string>& );
			int			m_execNames( Client&, const vector<string>& );
			int			m_execTime( Client&, const vector<string>& );
			int			m_execTopic( Client&, const vector<string>& );
			int			m_execUserhost( Client&, const vector<string>& );
			int			m_execVersion( Client&, const vector<string>& );
			int			m_execWall( Client&, const vector<string>& );
			int			m_execWallops( Client&, const vector<string>& );
			int			m_execWho( Client&, const vector<string>& );
			int			m_execRestart( Client&, const vector<string>& );
			void		m_init( int m_sockfd = -1 );

			bool		m_init_exec( void );

			int										m_sockfd;
			sockaddr_in6							m_addr;
			string									m_name,
													m_version,
													m_helpmsg,
													m_motd,
													m_password;
			vector< pollfd >						m_pollfd;
			map< int, Client >						m_clients;
			vector< Channel >						m_channels;

			map< int, string >						m_pending;
			map< int, queue< vector < string > > >	m_cmds;

			Trie< string >							m_operators;
			set< sockaddr_in6 >						m_banned;
			set< sockaddr_in6 >						m_muted;
			//vector< string >						m_command; // martin ajout ( gere par une variable non-membre )
			//int									m_commandId; // martin ajout ( gere par une variable non-membre )

			typedef int	(Server::*ExecFun)( Client&, const vector<string>& );
			static Trie< ExecFun > 					m_execs;

			static Trie< sockaddr_in6 >				m_hostnames;

			struct {
				bool	v6only,
						reuseaddr;
			}										m_opt;
	};

}
