/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   user.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsuardi <lsuardi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/11 14:27:27 by lsuardi           #+#    #+#             */
/*   Updated: 2022/06/11 14:36:29 by lsuardi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Server.hpp"

namespace irc
{

	void	Server::m_execUser( Client &sender, const vector<string> &arg )
	{
		ostringstream	response;

		if (m_isLogged(sender))
			response << m_prefix() << ERR_ALREADYREGISTRED << " * USER :You may not reregister" << m_endl();
		else if (arg.size() < 5)
			response << m_prefix() << ERR_NEEDMOREPARAMS << " * USER :Not enough parameters" << m_endl();
		else
		{
			sender.username = arg[1];
			sender.realname = arg[4];
			if (*sender.realname.data() == ':')
				sender.realname.erase(sender.realname.begin());
			for (size_t i = 5; i < arg.size(); ++i)
				sender.realname += " " + arg[i];
			m_attributeHost(sender);
			if (m_isLogged(sender))
			{
				m_pingClient(sender);
				m_welcome(sender);
			}
			return ;
		}
		m_appendToSend(sender.sockfd, response.str());
	}

}
