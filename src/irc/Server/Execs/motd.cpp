/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   motd.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsuardi <lsuardi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/11 14:27:27 by lsuardi           #+#    #+#             */
/*   Updated: 2022/06/11 14:53:25 by lsuardi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Server.hpp"

namespace irc
{

	void Server::m_execMotd( Client &sender, const vector<string> &arg )
	{
		vector<string>	motd_lines;
		ostringstream	response;

		static_cast<void>(arg);
		if (!m_isLogged(sender))
		{
			response << m_prefix() << ERR_NOTREGISTERED << " * MOTD :You have not registered" << m_endl();
		}
		else
		{
			try
			{
				motd_lines = split(m_getMotd(), '\n');
				response << m_prefix() << RPL_MOTDSTART << ' ' << sender.nickname << " :- " << m_name << " message of the day -" << m_endl();
				for (size_t i = 0; i < motd_lines.size(); ++i)
					response << m_prefix() << RPL_MOTD << ' ' << sender.nickname << " :- " << motd_lines[i] << m_endl();
				response << m_prefix() << RPL_ENDOFMOTD << ' ' << sender.nickname << " :End of /MOTD command" << m_endl();
			}
			catch (const MissingMotd&)
			{
				response << m_prefix() << ERR_NOMOTD << " * MOTD :Motd file missing" << m_endl();
			}
		}

		m_appendToSend(sender.sockfd, response.str());
	}

}
