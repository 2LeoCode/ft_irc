/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsuardi <lsuardi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/13 11:28:37 by lsuardi           #+#    #+#             */
/*   Updated: 2022/05/17 14:53:48 by lsuardi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils.hpp"

bool	operator <( const in6_addr &lhs, const in6_addr &rhs )
{
	return memcmp(&lhs, &rhs, sizeof(lhs)) < 0;
}

std::vector< std::string >	split( std::string s, char c )
{
	std::vector< std::string >	splitted;
	size_t	pos = 0;
	size_t	lastArg = s.find(':');

	while ((pos = s.find(c, pos)) != std::string::npos && pos < lastArg)
		s.replace(pos++, 1, " ");

	std::istringstream	is(s);

	while (!is.eof())
	{
		std::string s;
		is >> s;
		splitted.push_back(s);
	}
	return splitted;
}
