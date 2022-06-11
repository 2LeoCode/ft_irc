/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   join.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsuardi <lsuardi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/11 14:27:27 by lsuardi           #+#    #+#             */
/*   Updated: 2022/06/11 14:49:03 by lsuardi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Server.hpp"

namespace irc
{

	void		Server::m_execJoin( Client &sender, const vector<string> &arg )
	{
		vector< string >					chanNames;
		vector< string >					keys;
		vector< string >::const_iterator	currentKey;
		queue< Channel* >					chansToJoin;
		ostringstream 						response;

		if (arg.size() < 2)
			response << m_prefix() << ERR_NEEDMOREPARAMS << " * JOIN :Not enough parameters" << m_endl();
		else if (!m_isLogged(sender))
			response << m_prefix() << ERR_NOTREGISTERED << " * JOIN :You have not registered" << m_endl();
		else
		{
			chanNames = split(arg[1], ',');
			if (arg.size() > 2)
				keys = split(arg[2], ',');
			currentKey = keys.begin();
			typedef vector<string>::const_iterator Iter;
			for (Iter it = chanNames.begin(); it != chanNames.end(); ++it)
			{
				if (*it->data() != '#' && *it->data() != '&')
				{
					response << m_prefix() << ERR_NOSUCHCHANNEL << ' ' << *it << " :No such channel" << m_endl();
					continue ;
				}
				try
				{
					chansToJoin.push(&m_channels.at(*it));
				}
				catch (...)
				{
					if (*it->data() == '&' && !sender.hasMode(UMODE_OPERATOR))
						response << m_prefix() << ERR_NOSUCHCHANNEL << ' ' << *it << " :No such channel" << m_endl();
					else
					{
						m_channels.insert(make_pair(*it, Channel(*it)));
						chansToJoin.push(&m_channels.at(*it));
					}
				}
			}
			while (!chansToJoin.empty())
			{
				Channel	*cur = chansToJoin.front();
				chansToJoin.pop();

				if (sender.channelCount() == MAX_CHANNELS)
				{
					response << ERR_TOOMANYCHANNELS << ' ' << cur->name << " :You have joined to many channels" << m_endl();
					continue ;
				}
				if (cur->hasMode(CMODE_INVITEONLY) && !sender.isInvited(*cur))
				{
					response << m_prefix() << ERR_INVITEONLYCHAN << ' ' << cur->name << " :Cannot join channel (+i)" << m_endl();
					continue ;
				}
				if (cur->isBanned(sender))
				{
					response << m_prefix() << ERR_BANNEDFROMCHAN << ' ' << cur->name << " :Cannot join channel (+b)" << m_endl();
					continue ;
				}
				if (!cur->password.empty())
				{
					if (currentKey == keys.end())
					{
						goto BAD_KEY;
					}
					if (*currentKey++ != cur->password)
					{
						BAD_KEY:
						response << m_prefix() << ERR_BADCHANNELKEY << ' ' << cur->name << " :Cannot join channel (+k)" << m_endl();
						continue ;
					}
				}
				if (cur->userLimit != 0 && cur->users.size() == cur->userLimit)
				{
					response << m_prefix() << ERR_CHANNELISFULL << ' ' << cur->name << " :Cannot join channel (+l)" << m_endl();
					continue ;
				}
				sender.joinChannel(*cur);
				if (cur->users.size() == 1)
					cur->op(sender);
				if (!cur->topic.empty())
					response << m_prefix() << RPL_TOPIC << ' ' << cur->name << " :" << cur->topic << m_endl();
				m_appendToSend(sender.sockfd, response.str());

				ostringstream info;
				info << sender.makePrefix() << "JOIN :" << cur->name << m_endl();
				typedef set< const Client* >::const_iterator Iter2;
				for (Iter2 it2 = cur->users.begin(); it2 != cur->users.end(); ++it2)
					m_appendToSend((*it2)->sockfd, info.str());
				m_execNames(sender, m_make_args(2, "NAMES", cur->name.data()));
			}
		}
		m_appendToSend(sender.sockfd, response.str());
	}

}
