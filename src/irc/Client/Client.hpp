/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Leo Suardi <lsuardi@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/16 00:53:55 by Leo Suardi        #+#    #+#             */
/*   Updated: 2022/03/24 00:25:46 by Leo Suardi       ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

namespace irc
{

	class Client
	{
		public:
			Client( void );
			~Client( void );


		private:
			std::string	m_username;
	};

}
