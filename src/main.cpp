/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Leo Suardi <lsuardi@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/22 23:58:32 by Leo Suardi        #+#    #+#             */
/*   Updated: 2022/04/23 17:57:14 by Leo Suardi       ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <sstream>
#include <cstdlib>
#include <climits>

#include "data.hpp"
#include "net.hpp"

int	main( int argc, char **argv )
{
	char						*endptr;
	long						port;
	std::string					password;
	net::TCPServer				serv;
	std::string					command;
	std::vector< TCPClient >	clients;

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
		serv.open().bind(port).listen();
		while (true) {
			std::string	request;
			unsigned id = serv.poll();

			if (id == 0)
				clients.push_back(serv.accept());
			else {
				int fd = serv.recv(id);

				if (fd) {
					typedef std::vector< TCPClient >::const_iterator iterator;

					for (iterator it = clients.begin(); it != clients.end(); ++it) {
						if (it->sockfd == fd)
							clients.erase(it);
					}
				}
			}
			serv.handle_pending();
		}
	} 
	catch (const std::exception &e)
	{
		std::cerr << e.what() << std::endl;
	}
	return 0;
}
