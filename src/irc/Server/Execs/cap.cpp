/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cap.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsuardi <lsuardi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/11 14:27:27 by lsuardi           #+#    #+#             */
/*   Updated: 2022/06/11 14:35:31 by lsuardi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Server.hpp"

namespace irc
{

	void	Server::m_execCap( Client &sender, const vector<string> &arg )
	{
		ostringstream	response;

		if (arg.size() < 2)
			response << m_prefix() << ERR_NEEDMOREPARAMS << " * CAP :Not enough parameters" << m_endl();
		else if (arg[1] == "LS")
			response << m_prefix() << "CAP * LS :" << m_endl();
		else if (arg[1] == "END")
			return ;
		else
			response << m_prefix() << ERR_INVALIDCAPCMD << " * " << arg[1] << " :Invalid CAP sub-command" << m_endl();
		m_appendToSend(sender.sockfd, response.str());
	}

}
