/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pong.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsuardi <lsuardi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/11 14:27:27 by lsuardi           #+#    #+#             */
/*   Updated: 2022/06/11 14:52:19 by lsuardi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Server.hpp"

namespace irc
{

	void Server::m_execPong( Client &sender, const vector< string > &arg )
	{
		ostringstream		response;
		typedef list< pair< int, pair< string, timespec > > >::iterator iter;

		if (arg.size() < 2)
			response << m_prefix() << ERR_NEEDMOREPARAMS << " * PONG :Not enough parameters" << m_endl();
		else
		{
			iter it = m_pings.begin();
			while (it != m_pings.end() && it->first != sender.sockfd)
				++it;
			if (it != m_pings.end())
			{
				if (it->second.first == arg[1])
					m_pings.erase(it);
			}
			return ;
		}
		m_appendToSend(sender.sockfd, response.str());
	}

}
