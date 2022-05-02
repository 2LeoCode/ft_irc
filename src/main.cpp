/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Leo Suardi <lsuardi@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/22 23:58:32 by Leo Suardi        #+#    #+#             */
/*   Updated: 2022/05/02 03:43:15 by Leo Suardi       ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <sstream>
#include <cstdlib>
#include <climits>

#include "data.hpp"
#include "net.hpp"
#include "irc.hpp"

using irc::Server;
using std::string;
using std::cout;
using std::endl;

int	main( int argc, char **argv )
{
	char	*endptr;
	long	port;
	string	password;

	if (argc < 3)
	{
		std::cerr << "usage: " << *argv << " <port> <password>" << std::endl;
		return 1;
	}
	port = std::strtol(argv[1], &endptr, 0);
	if (port <= 0 || port > USHRT_MAX || *endptr)
	{
		std::cerr << "invalid port `" << argv[1] << '\'' << std::endl;
		return 1;
	}
	for (int i = 2; i < argc - 1; ++i)
		(password += argv[i]) += ' ';
	password += argv[argc - 1];

	try
	{
		Server(port, password).loop();
	} 
	catch (const std::exception &e)
	{
		cout << e.what() << endl;
		return -1;
	}
	catch (Server::ShutdownEvent &e)
	{
		cout << "Server is shutting down" << endl;
	}
	return 0;
}
