/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsuardi <lsuardi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/15 20:33:28 by lsuardi           #+#    #+#             */
/*   Updated: 2022/03/15 21:25:29 by lsuardi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "../Client/Client.hpp"
#include "../../Trie/Trie.hpp"

namespace IRC {
	class Server {
		public:
			Server( void );
			~Server( void );


		private:
			Trie< Client >	m_clients;	// Client list
			int				m_sockfd;	// Socket FD
	};
}
