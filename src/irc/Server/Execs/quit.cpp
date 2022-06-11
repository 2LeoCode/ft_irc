/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quit.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsuardi <lsuardi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/11 14:27:27 by lsuardi           #+#    #+#             */
/*   Updated: 2022/06/11 14:52:43 by lsuardi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Server.hpp"

namespace irc
{

	void Server::m_execQuit( Client &sender, const vector< string > &arg )
	{
		ostringstream response;

		(void)arg;
		if (!m_isLogged(sender))
			response << m_prefix() << ERR_NOTREGISTERED << " * QUIT :You have not registered" << m_endl();
		else
		{
			m_kickClient(sender);
			return ;
		}
		m_appendToSend(sender.sockfd, response.str());
	}

}
