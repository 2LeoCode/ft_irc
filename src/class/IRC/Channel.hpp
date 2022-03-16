/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Leo Suardi <lsuardi@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/16 00:58:14 by Leo Suardi        #+#    #+#             */
/*   Updated: 2022/03/16 01:00:01 by Leo Suardi       ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>

namespace IRC {
	class Channel {
		public:
			Channel( void );
			~Channel( void );


		private:
			std::string		m_name;
			Trie< Client >	m_ops;
			Trie< Client >	m_members;
	};
}
