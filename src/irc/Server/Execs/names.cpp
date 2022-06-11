/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   names.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsuardi <lsuardi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/11 14:27:27 by lsuardi           #+#    #+#             */
/*   Updated: 2022/06/11 14:51:46 by lsuardi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Server.hpp"

namespace irc
{

	void	Server::m_execNames( Client &sender, const vector< string > &arg )
	{
		typedef set< const Client* >::const_iterator	Iter;
		ostringstream						response;
		vector< string >					channels;
		vector< string >::const_iterator	curChannel;

		if (arg.size() < 2)
			response << m_prefix() << ERR_NEEDMOREPARAMS << " * NAMES :Not enough parameters" << m_endl();
		else if (!m_isLogged(sender))
			response << m_prefix() << ERR_NOTREGISTERED << " * NAMES :You have not registered" << m_endl();
		else
		{
			channels = split(arg[1], ',');
			curChannel = channels.begin();
			while (curChannel != channels.end())
			{
				const Channel	*c;
				try
				{
					c = &m_channels.at(*curChannel);
				}
				catch (...)
				{ continue ; }
				response << m_prefix() << RPL_NAMREPLY << ' ' << sender.nickname << " = " << *curChannel << " :";
				Iter it = c->users.begin();
				while (it != c->users.end())
				{
					if (c->isOperator(**it))
						response << '@';
					response << (*it)->nickname;
					if (++it != c->users.end())
						response << ' ';
				}
				response << m_endl() << m_prefix() << RPL_ENDOFNAMES << ' ' << sender.nickname << ' ' << *curChannel << " :End of /NAMES list" << m_endl();
				++curChannel;
			}
		}
		m_appendToSend(sender.sockfd, response.str());
	}

}
