/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsuardi <lsuardi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/27 16:24:24 by Leo Suardi        #+#    #+#             */
/*   Updated: 2022/05/14 18:16:03 by lsuardi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <sys/socket.h>
#include <sys/poll.h>
#include <netinet/in.h>

#include "../../irc.hpp"

namespace irc {

	class Server {
		public:
			Server( void );
			Server( string, short, string, int = 0, int = 32 );
			~Server();

			Server		&open( string, short, string, int = 0, int = 32 );
			void		loop( void );


			struct ShutdownEvent { }; // We throw this object to turn off the server
			struct ClientDisconnectEvent { };

		private:

			void		m_getMotd( void );
			void		m_getHelpMsg( void );
			void		m_getOps( void );
			bool		m_isLogged( const Client& ) const;
			void		m_send( int, const std::string& );
			//int		send( const std::string&, const std::string& ); Implementer si besoin
			void		m_recv( int );
			void		m_parsePending( void );
			void		m_execCommandQueues( void );
			void		m_kickClient( Client& );
			Client		&m_findClient( const std::string &name );

			vector< string >	m_parseCommand( const std::string& ); // martin ajout
			int					m_execCommand( Client&, const vector< string >& ); // martin ajout

			void		m_appendToSend( int, const string& );

			// added
			void		m_execCap( Client&, const vector<string>& );
			void		m_execPass( Client&, const vector<string>& );
			void		m_execNick( Client&, const vector<string>& );
			void		m_execUser( Client&, const vector<string>& );
			void		m_execPing( Client&, const vector<string>& );
			void		m_execOper( Client&, const vector<string>& );
			void		m_execDie( Client&, const vector<string>& );
			void		m_execKill( Client&, const vector<string>& );

			// to be added
			void		m_execJoin( Client&, const vector<string>& );
			void		m_execList( Client&, const vector<string>& );
			void		m_execInvite( Client&, const vector<string>& );
			void		m_execGlobops( Client&, const vector<string>& );
			void		m_execImportmotd( Client&, const vector<string>& );
			void		m_execIsbanned( Client&, const vector<string>& );
			void		m_execIson( Client&, const vector<string>& );
			void		m_execKillban( Client&, const vector<string>& );
			void		m_execUnban( Client&, const vector<string>& );
			void		m_execMode( Client&, const vector<string>& );
			void		m_execKick( Client&, const vector<string>& );
			void		m_execSetname( Client&, const vector<string>& );
			void		m_execPart( Client&, const vector<string>& );
			void		m_execQuit( Client&, const vector<string>& );
			void		m_execMe( Client&, const vector<string>& );
			void		m_execPrivmsg( Client&, const vector<string>& );
			void		m_execNotice( Client&, const vector<string>& );
			void		m_execNames( Client&, const vector<string>& );
			void		m_execTime( Client&, const vector<string>& );
			void		m_execTopic( Client&, const vector<string>& );
			void		m_execUserhost( Client&, const vector<string>& );
			void		m_execVersion( Client&, const vector<string>& );
			void		m_execWall( Client&, const vector<string>& );
			void		m_execWallops( Client&, const vector<string>& );
			void		m_execWho( Client&, const vector<string>& );
			void		m_execRehash( Client&, const vector<string>& );
			void		m_execShun( Client&, const vector<string>& );
			void		m_execHelp( Client&, const std::vector<string>& );
			void		m_execInfo( Client&, const vector<string>& );

			int										m_sockfd;
			sockaddr_in6							m_addr;
			string									m_name,
													m_helpmsg,
													m_motd,
													m_password;
			vector< pollfd >						m_pollfd;
			map< int, Client >						m_clients;
			map< string, Channel >					m_channels;

			map< int, string >						m_pending;
			map< int, string >						m_tosend;
			map< int, queue< vector < string > > >	m_cmds;

			Trie< string >							m_operators;
			set< string >							m_banned;
			set< string	>							m_muted;
			//vector< string >						m_command; // martin ajout ( gere par une variable non-membre )
			//int									m_commandId; // martin ajout ( gere par une variable non-membre )

			typedef void	(Server::*ExecFun)( Client&, const vector<string>& );
			Trie< ExecFun > 						m_execs;

			map< sockaddr_in6, string >				m_hostnames;

			struct {
				int		reuseaddr;
			}										m_opt;
	};

}
