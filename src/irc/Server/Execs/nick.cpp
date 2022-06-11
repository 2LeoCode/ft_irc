/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   nick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsuardi <lsuardi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/11 14:27:27 by lsuardi           #+#    #+#             */
/*   Updated: 2022/06/11 14:35:59 by lsuardi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Server.hpp"

namespace irc
{

	void	Server::m_execNick( Client &sender, const vector<string> &arg )
	{
		ostringstream	response;
		bool			initLog = m_isLogged(sender);

		if (arg.size() < 2)
			response << m_prefix() << ERR_NONICKNAMEGIVEN << " * NICK :No nickname given" << m_endl();
		else if (arg[1].length() > 9 || arg[1].find_first_not_of(g_nickCharset) != string::npos)
			response << m_prefix() << ERR_ERRONEUSNICKNAME << " * " << arg[1] << " :Erroneus nickname" << m_endl();
		else
		{
			map<int, Client>::const_iterator it = m_clients.begin();
			while (it != m_clients.end())
			{
				if (it->second.sockfd != sender.sockfd && it->second.nickname == arg[1])
				{
					response << m_prefix() << ERR_NICKNAMEINUSE << " * " << arg[1] << " :Nickname is already in use" << m_endl();
					break ;
				}
				++it;
			}
			if (it == m_clients.end())
			{
				sender.nickname = arg[1];
				if (!initLog && m_isLogged(sender))
				{
					m_pingClient(sender);
					m_welcome(sender);
				}
				return ;
			}
		}
		m_appendToSend(sender.sockfd, response.str());
	}

}
