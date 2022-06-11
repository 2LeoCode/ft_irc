/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsuardi <lsuardi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/11 14:27:27 by lsuardi           #+#    #+#             */
/*   Updated: 2022/06/11 14:36:41 by lsuardi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Server.hpp"

namespace irc
{

	void	Server::m_execPing( Client &sender, const vector<string> &arg )
	{
		ostringstream	response;

		if (arg.size() < 2)
			response << m_prefix() << ERR_NEEDMOREPARAMS << " * PING :Not enough parameters" << m_endl();
		else if (!m_isLogged(sender))
			response << m_prefix() << ERR_NOTREGISTERED << " * PING :You have not registered" << m_endl();
		else
			response << "PONG :" << arg[1] << m_endl();
		m_appendToSend(sender.sockfd, response.str());
	}

}
