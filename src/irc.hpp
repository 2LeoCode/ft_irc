/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   irc.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Leo Suardi <lsuardi@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/11 14:26:16 by lsuardi           #+#    #+#             */
/*   Updated: 2022/05/02 20:43:02 by Leo Suardi       ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

namespace irc
{
	using std::string;
	using std::vector;
	using std::map;
	using std::queue;
	using std::cout;
	using std::endl;
}

#include "irc/Channel/Channel.hpp"
#include "irc/Client/Client.hpp"
#include "irc/Server/Server.hpp"
