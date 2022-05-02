/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Leo Suardi <lsuardi@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/16 00:58:14 by Leo Suardi        #+#    #+#             */
/*   Updated: 2022/05/02 03:32:25 by Leo Suardi       ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include "../../data/Trie/Trie.hpp"
#include "../Client/Client.hpp"

namespace irc
{

	class Channel
	{
		public:
			Channel( void );
			~Channel( void );


		private:
			string					m_name;
			map< string, Client >	m_users;
			data::Trie< Client >	m_users;
	};

}
