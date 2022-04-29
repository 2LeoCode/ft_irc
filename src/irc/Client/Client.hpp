/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Leo Suardi <lsuardi@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/16 00:53:55 by Leo Suardi        #+#    #+#             */
/*   Updated: 2022/04/27 16:24:04 by Leo Suardi       ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <netinet/in.h>

using std::string;

namespace irc
{

	class Client
	{
		public:
			Client( void );
			~Client( void );


		private:
			int				m_sockfd;
			sockaddr_in6*	m_addr;
			string			m_username,
							m_nickname,
							m_password;
	};

}
