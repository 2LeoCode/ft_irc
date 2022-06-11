/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   privmsg.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsuardi <lsuardi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/11 14:27:27 by lsuardi           #+#    #+#             */
/*   Updated: 2022/06/11 14:50:01 by lsuardi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Server.hpp"

namespace irc
{

	void	Server::m_execPrivmsg(Client &sender, const vector<string> &arg)
	{
		ostringstream response;
		vector< const Client* > privTargets;
		vector< pair < const Client*, const string > > chanTargets;

		if (!m_isLogged(sender))
		{
			response << m_prefix() << " * " << arg[0] << " :You have not registered" << m_endl();
		}
		else if (arg.size() == 1)
		{
			response << m_prefix() << ERR_NORECIPIENT << " * " << arg[0] << " :No recipient given (" << arg[0] << ")" << m_endl();
		}
		else if (arg.size() == 2)
		{
			response << m_prefix() << ERR_NOTEXTTOSEND << " * PRIVMSG :No text to send (PRIVMSG)" << m_endl();
		}
		else
		{
			typedef vector< string >::iterator iter;
			vector< string > receivers = split(arg[1], ',');
			for (iter it = receivers.begin(); it != receivers.end(); it++)
			{
				if (*it->data() == '#' || *it->data() == '&')
				{
					//push every client of channel to chanTargets
					try
					{
						typedef set< const irc::Client* >::iterator cliIter;
						Channel *ptr = &m_channels.at(*it);
						if (ptr->canSpeak(sender))
						{
							for (cliIter cliIt = ptr->users.begin(); cliIt != ptr->users.end(); cliIt++)
							{
								if ((*cliIt)->sockfd != sender.sockfd)
									chanTargets.push_back(make_pair(*cliIt, ptr->name));
							}
						}
						else
						{
							response << m_prefix() << ERR_CANNOTSENDTOCHAN << ' ' << ptr->name << " :Cannot send to channel" << m_endl();
						}
					}
					catch (...)
					{
						response << m_prefix() << ERR_NOSUCHNICK << ' ' << *it << " :No such nick/channel" << m_endl();
					}
				}
				else
				{
					privTargets.push_back(&m_findClient(*it));
				}
			}
			//loop through target
			typedef vector< const irc::Client* >::iterator privIter;
			for (privIter privIt = privTargets.begin(); privIt != privTargets.end(); privIt++)
			{
				string request = sender.makePrefix() + arg[0] + " " + (*privIt)->nickname + " :" + arg[2] + m_endl();
				m_appendToSend((*privIt)->sockfd, request);
			}
			typedef vector< pair < const irc::Client*, const string > >::iterator chanIter;
			for (chanIter chanIt = chanTargets.begin(); chanIt != chanTargets.end(); chanIt++)
			{
				string request = sender.makePrefix() + arg[0] + " " + chanIt->second + " :" + arg[2] + m_endl();
				m_appendToSend(chanIt->first->sockfd, request);
				// send message
			}
		}
		m_appendToSend(sender.sockfd, response.str());
	}

}
