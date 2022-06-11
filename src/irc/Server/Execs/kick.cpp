/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsuardi <lsuardi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/11 14:27:27 by lsuardi           #+#    #+#             */
/*   Updated: 2022/06/11 14:53:03 by lsuardi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Server.hpp"

namespace irc
{

	void Server::m_execKick( Client &sender, const vector< string > &arg )
	{
		ostringstream response;

		if (arg.size() < 3)
			response << m_prefix() << ERR_NEEDMOREPARAMS << " * KICK :Not enough parameters" << m_endl();
		else if (!m_isLogged(sender))
			response << m_prefix() << ERR_NOTREGISTERED << " * KICK :You have not registered" << m_endl();
		else
		{
			vector< string >	chanNames, userNames;
			vector< Channel* >	chans;

			chanNames = split(arg[1], ','); userNames = split(arg[2], ',');
			chans.reserve(chanNames.size());
			for (size_t i = 0; i < chanNames.size(); ++i)
			{
				try
				{
					chans.push_back(&m_channels.at(chanNames[i]));
				}
				catch (...)
				{
					response << m_prefix() << ERR_NOSUCHCHANNEL << ' ' << chanNames[i] << " :No such channel" << m_endl();
				}
			}
			for (size_t i = 0; i < userNames.size(); ++i)
			{
				Client	*user;

				try
				{
					user = &m_findClient(userNames[i]);
				}
				catch (...)
				{
					response << m_prefix() << ERR_NOSUCHNICK << ' ' << userNames[i] << " :No such nick" << m_endl();
					continue ;
				}
				size_t j = 0;
				while (j < chans.size())
				{
					if (chans[j]->hasClient(*user))
						break ;
				}
				if (j != chans.size())
				{
					if (!sender.hasMode(UMODE_OPERATOR))
					{
						if (!chans[j]->hasClient(sender))
							response << m_prefix() << ERR_NOTONCHANNEL << ' ' << chans[j]->name << " :You're not on that channel" << m_endl();
						else if (!chans[j]->isOperator(sender))
							response << m_prefix() << ERR_CHANOPRIVSNEEDED << ' ' << chans[j]->name << " :You're not channel operator" << m_endl();
						else
							goto DO_KICK;
						continue ;
					}
					DO_KICK:
					ostringstream info;

					info << sender.makePrefix() << "KICK " << chans[j]->name << ' ' << user->nickname << " :";
					if (arg.size() > 3)
						info << arg[3];
					info << m_endl();

					typedef set< const Client* >::const_iterator ChanIter;
					for (ChanIter it = chans[j]->users.begin(); it != chans[j]->users.end(); ++it)
						m_appendToSend((*it)->sockfd, info.str());
					user->partChannel(*chans[j]);
				}
			}
		}
		m_appendToSend(sender.sockfd, response.str());
	}

}
