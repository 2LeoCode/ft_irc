/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   die.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsuardi <lsuardi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/11 14:27:27 by lsuardi           #+#    #+#             */
/*   Updated: 2022/06/11 14:39:17 by lsuardi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Server.hpp"

namespace irc
{

	void	Server::m_execDie( Client &sender, const vector<string> &arg )
	{
		ostringstream response;

		(void)arg;
		if (!m_isLogged(sender))
			response << m_prefix() << ERR_NOTREGISTERED << " * DIE :You have not registered" << m_endl();
		else if (!sender.hasMode(UMODE_OPERATOR))
			response << m_prefix() << ERR_NOPRIVILEGES << " * :Permission Denied- You're not an IRC operator" << m_endl();
		else
			throw (ShutdownEvent());
		m_appendToSend(sender.sockfd, response.str());
	}

}
