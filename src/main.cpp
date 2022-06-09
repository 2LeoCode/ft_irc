/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Leo Suardi <lsuardi@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/22 23:58:32 by Leo Suardi        #+#    #+#             */
/*   Updated: 2022/06/09 13:10:22 by Leo Suardi       ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "data.hpp"
#include "irc.hpp"
#include <csignal>

using irc::Server;
using std::string;
using std::strtol;
using std::exception;
using std::cout;
using std::cerr;
using std::endl;

void INT_handler( int sig )
{
	(void)sig;
	cout << "\r";
	throw Server::ShutdownEvent();
}

int	main( int argc, char **argv )
{
	char				*endptr;
	long				port;
	string				password;
	struct sigaction	act;

	srandom(time(NULL));
	act.sa_handler = INT_handler;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	if (sigaction(SIGINT, &act, NULL))
	{
		cout << "Exception caught: " << strerror(errno) << endl;
		return 1;
	}
	if (argc < 3)
	{
		cerr << "usage: " << *argv << " <port> <password>" << endl;
		return 1;
	}
	port = strtol(argv[1], &endptr, 0);
	if (port <= 0 || port > USHRT_MAX || *endptr)
	{
		cerr << "invalid port `" << argv[1] << '\'' << endl;
		return 1;
	}
	for (int i = 2; i < argc - 1; ++i)
		(password += argv[i]) += ' ';
	password += argv[argc - 1];

	try
	{
		Server("KEKserv", port, password).loop();
	}
	catch (const exception &e)
	{
		cout << "Exception caught: " << e.what() << endl;
		return -1;
	}
	catch (Server::ShutdownEvent &e)
	{
		cout << "Server is shutting down..." << endl;
	}
	return 0;
}
