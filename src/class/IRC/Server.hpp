/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Leo Suardi <lsuardi@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/15 20:33:28 by lsuardi           #+#    #+#             */
/*   Updated: 2022/03/16 00:58:09 by Leo Suardi       ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Client.hpp"
#include "Channel.hpp"
#include "../Trie/Trie.hpp"

namespace IRC {
	class Server {
		public:
			Server( void );
			~Server( void );


		private:
			Trie< Channel >	m_channels;
			Trie< Client >	m_clients;	// Client list
			int				m_sockfd;	// Socket FD
	};
}
