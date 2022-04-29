/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: martin <martin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/27 16:24:24 by Leo Suardi        #+#    #+#             */
/*   Updated: 2022/04/29 18:04:25 by martin           ###   ########.fr       */
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

			void		parseCommand(const std::string &command); // martin ajout
			int			execCommand(); // martin ajout

			// martin ajout
			int			execJoin(const std::vector<std::string> &command);
			int			execKick(const std::vector<std::string> &command);
			int			execNick(const std::vector<std::string> &command);
			int			execSetname(const std::vector<std::string> &command);
			int			execPart(const std::vector<std::string> &command);
			int			execPass(const std::vector<std::string> &command);
			int			execPrivmsg(const std::vector<std::string> &command);
			int			execQuit(const std::vector<std::string> &command);
			int			execUser(const std::vector<std::string> &command);
			int			execOper(const std::vector<std::string> &command);

			const static std::string cmdTab[10]; //martin ajout


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
			std::vector< std::string >		m_command; // martin ajout
			int								m_commandId; // martin ajout
			struct {
				bool v6only, reuseaddr, fionbio;
			}								m_opt;
	};

}
