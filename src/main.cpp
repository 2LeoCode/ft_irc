/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsuardi <lsuardi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/22 23:58:32 by Leo Suardi        #+#    #+#             */
/*   Updated: 2022/04/12 16:38:49 by lsuardi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <sstream>
#include <cstdlib>
#include <climits>

#include "data.hpp"
#include "net.hpp"
int	trie_test_2( void );

int	main( int argc, char **argv )
{
	trie_test_2();
	char		*endptr;
	long		port;
	std::string	password;
	net::Socket	sock;
	std::string	command;
	
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
		sock.open().bind(port).listen();
		while (true)
		{
			sock.poll();
			if (sock.is_server())
				sock.accept();
			else
				sock.recv();
			while ((command = get_next_cmd()) != "")
			{
				exec_command(command)
				send_response();
			}
		}
	} 
	catch (const std::exception &e)
	{
		std::cerr << e.what() << std::endl;
	}
	return 0;
}
