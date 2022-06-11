/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   oper.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsuardi <lsuardi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/11 14:27:27 by lsuardi           #+#    #+#             */
/*   Updated: 2022/06/11 14:37:11 by lsuardi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Server.hpp"

namespace irc
{

	void	Server::m_execOper ( Client &sender, const vector<string> &arg )
	{
		ostringstream	response;

		if (arg.size() < 3)
			response << m_prefix() << ERR_NEEDMOREPARAMS << " * OPER :Not enough parameters" << m_endl();
		else if (!m_isLogged(sender))
			response << m_prefix() << ERR_NOTREGISTERED << " * OPER :You have not registered" << m_endl();
		else if (m_operators.at(arg[1].data()) != arg[2])
			response << m_prefix() << ERR_PASSWDMISMATCH << " * OPER :Password incorrect" << m_endl();
		else
		{
			sender.addMode(UMODE_OPERATOR);
			response << m_prefix() << RPL_YOUREOPER << " :You are now an IRC operator" << m_endl();
		}
		m_appendToSend(sender.sockfd, response.str());
	}

}
