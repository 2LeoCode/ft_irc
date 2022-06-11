/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   part.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsuardi <lsuardi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/11 14:27:27 by lsuardi           #+#    #+#             */
/*   Updated: 2022/06/11 14:51:53 by lsuardi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Server.hpp"

namespace irc
{

	void Server::m_execPart( Client &sender, const vector< string > &arg )
	{
		ostringstream		response;
		vector< string >	channels;

		if (arg.size() < 2)
			response << m_prefix() << ERR_NEEDMOREPARAMS << " * PART :Not enough parameters" << m_endl();
		else if (!m_isLogged(sender))
			response << m_prefix() << ERR_NOTREGISTERED << " * PART :You have not registered" << m_endl();
		else
		{
			channels = split(arg[1], ',');
			for (size_t i = 0; i < channels.size(); i++)
			{
				map< string, Channel >::iterator it = m_channels.find(channels[i]);
				if (it == m_channels.end())
					response << m_prefix() << ERR_NOSUCHCHANNEL << ' ' << channels[i] << " :No such channel" << m_endl();
				else if (!it->second.hasClient(sender))
					response << m_prefix() << ERR_NOTONCHANNEL << ' ' << channels[i] << " :You're not on that channel" << m_endl();
				else
				{
					ostringstream info;
					info << sender.makePrefix() << "PART :" << it->second.name << m_endl();
					typedef set< const Client* >::iterator	Iter;
					for (Iter it2 = it->second.users.begin(); it2 != it->second.users.end(); ++it2)
						m_appendToSend((*it2)->sockfd, info.str());
					sender.partChannel(it->second);
					// EXIT CHANNEL
					if (it->second.empty())
						m_channels.erase(it);
						// Erase channel if no one is on it
				}
			}
		}
		m_appendToSend(sender.sockfd, response.str());
	}

}
