/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pass.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsuardi <lsuardi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/11 14:27:27 by lsuardi           #+#    #+#             */
/*   Updated: 2022/06/11 14:35:44 by lsuardi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Server.hpp"

namespace irc
{

	void	Server::m_execPass( Client &sender, const vector<string> &arg )
	{
		ostringstream	response;

		if (m_isLogged(sender))
			response << m_prefix() << ERR_ALREADYREGISTRED << " * PASS :You may not reregister" << m_endl();
		else if (arg.size() < 2)
			response << m_prefix() << ERR_NEEDMOREPARAMS << " * PASS :Not enough parameters" << m_endl();
		else
		{
			sender.lastpass = arg[1];
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
