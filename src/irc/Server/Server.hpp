/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Leo Suardi <lsuardi@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/27 16:24:24 by Leo Suardi        #+#    #+#             */
/*   Updated: 2022/06/11 15:34:18 by Leo Suardi       ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#ifdef DEBUG
# include <ncurses.h>
#endif

#include <sys/socket.h>
#include <sys/poll.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <cerrno>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <ctime>

#include "../../irc.hpp"

#define PING_TIMEOUT 1

namespace irc {

	class Server {
		public:

#ifdef DEBUG
			void	debug(void);
			void	init_debug(void);
			void	display_info(WINDOW *window);
		private:
			WINDOW									*win;
		public:
#endif

			Server( void );
			Server( string, short, string, int = 0, int = 32 );
			~Server();

			Server		&open( string, short, string, int = 0, int = 32 );
			void		loop( void );


			struct ShutdownEvent { }; // We throw this object to turn off the server
			struct ClientDisconnectEvent { };
			EXCEPTION(MissingMotd, "Motd file missing");


		private:

			string		m_getMotd( void );
			void		m_getHelpMsg( void );
			void		m_getOps( void );
			bool		m_isLogged( const Client& ) const;
			void		m_send( int, const std::string& );
			void		m_recv( int );
			void		m_parsePending( void );
			void		m_execCommandQueues( void );
			void		m_kickClient( Client& );
			Client		&m_findClient( const string& );
			Client		&m_findClientByHost( const string & );
			void		m_attributeHost( Client& );

			vector< string >	m_parseCommand( const string& );
			int					m_execCommand( Client&, const vector< string >& );

			void		m_appendToSend( int, const string& );
			void		m_pingClient( Client &c );

			// added
			void		m_execCap( Client&, const vector<string>& );
			void		m_execPass( Client&, const vector<string>& );
			void		m_execNick( Client&, const vector<string>& );
			void		m_execUser( Client&, const vector<string>& );
			void		m_execPing( Client&, const vector<string>& );
			void		m_execOper( Client&, const vector<string>& );
			void		m_execDie( Client&, const vector<string>& );
			void		m_execKill( Client&, const vector<string>& );
			void		m_execJoin( Client&, const vector<string>& );
			void		m_execPrivmsg( Client&, const vector<string>& );
			void		m_execNames( Client&, const vector<string>& );
			void		m_execPart( Client&, const vector<string>& );
			void 		m_execPong( Client&, const vector< string >& );
			void		m_execNotice( Client&, const vector<string>& );
			void		m_execMode( Client&, const vector<string>& );
			void		m_execKick( Client&, const vector<string>& );
			void		m_execMotd( Client&, const vector<string>& );
			void		m_execQuit( Client&, const vector<string>& );

			void		m_welcome( Client& );

			const string			m_prefix( void ) const;
			const string			m_endl( void ) const;
			const vector< string >	m_make_args( int, ... ) const;
			int										m_sockfd;
			sockaddr_in6							m_addr;
			string									m_name,
													m_helpmsg,
													m_password;
			vector< pollfd >						m_pollfd;

			list< pair< int, pair< string, timespec > > >	m_pings;

			map< int, string >						m_pending;
			map< int, string >						m_tosend;
			map< int, queue< vector < string > > >	m_cmds;

			Trie< string >							m_operators;
			set< string >							m_banned;
			set< string	>							m_muted;

			typedef void	(Server::*ExecFun)( Client&, const vector<string>& );
			Trie< ExecFun > 						m_execs;

			map< string, Channel >					m_channels;
			map< int, Client >						m_clients;
			map< string, in6_addr >					m_hostnames;
			time_t									m_startTime;

			struct {
				int		reuseaddr;
			}										m_opt;

	};

}
