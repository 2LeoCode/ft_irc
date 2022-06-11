/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kill.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsuardi <lsuardi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/11 14:27:27 by lsuardi           #+#    #+#             */
/*   Updated: 2022/06/11 14:55:37 by lsuardi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Server.hpp"

namespace irc
{

	void    Server::m_execKill( Client &sender, const vector<string> &arg )
	{
		ostringstream response;

		if (arg.size() < 2)
			response << m_prefix() << ERR_NEEDMOREPARAMS << " * KILL :Not enough parameters" << m_endl();
		else if (!m_isLogged(sender))
			response << m_prefix() << ERR_NOTREGISTERED << " * KILL :You have not registered" << m_endl();
		else if (!sender.hasMode(UMODE_OPERATOR))
			response << m_prefix() << ERR_NOPRIVILEGES << " * KILL :Permission Denied- You're not an IRC operator" << m_endl();
		else if (arg[1] == m_name)
			response << m_prefix() << ERR_CANTKILLSERVER << " * KILL :you cant kill a server!" << m_endl(); 
		else
		{
			try
			{
				m_kickClient(m_findClient(arg[1]));
			}
			catch (...)
			{
				response << m_prefix() << ERR_NOSUCHNICK << ' ' << arg[1] << "No such nick" << m_endl();
			}
		}
		m_appendToSend(sender.sockfd, response.str());
	}

}
