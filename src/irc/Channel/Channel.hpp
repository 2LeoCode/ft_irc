/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsuardi <lsuardi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/16 00:58:14 by Leo Suardi        #+#    #+#             */
/*   Updated: 2022/04/11 14:21:11 by lsuardi          ###   ########.fr       */
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
			std::string				m_name;
			data::Trie< Client >	m_users;
	};

}
