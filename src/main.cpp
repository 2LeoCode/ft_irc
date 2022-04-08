/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Leo Suardi <lsuardi@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/22 23:58:32 by Leo Suardi        #+#    #+#             */
/*   Updated: 2022/03/31 20:41:12 by Leo Suardi       ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <sstream>
#include <cstdlib>
#include <climits>

int	main( int argc, char **argv )
{
	char		*endptr;
	int			port;
	std::string	password;

	if (argc < 3) {
		std::cerr << "usage: " << *argv << " <port> <password>" << std::endl;
		return 1;
	}
	port = std::strtol(argv[1], &endptr, 0);
	if (port <= 0 || port > USHRT_MAX || *endptr) {
		std::cerr << "invalid port `" << argv[1] << '\'' << std::endl;
		return 1;
	}
	for (int i = 2; i < argc - 1; ++i)
		(password += argv[i]) += ' ';
	password += argv[argc - 1];
	return 0;
}
