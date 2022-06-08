/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Leo Suardi <lsuardi@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/13 11:28:37 by lsuardi           #+#    #+#             */
/*   Updated: 2022/06/08 18:13:55 by Leo Suardi       ###   ########.fr       */
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

std::string randomString( size_t size )
{
	std::string ret;
	long randNum;
	char *ptr;

	while (ret.size() < size)
	{
		randNum = random();
		ptr = reinterpret_cast< char* >(&randNum);
		for (size_t i = 0; ret.size() < size && i < sizeof(long) / sizeof(char); ++i)
		{
			if (isgraph(*ptr))
				ret += *ptr;
			randNum <<= sizeof(char) * 8;
		}
	}
	return ret;
}

bool operator <( const timespec &lhs, const timespec &rhs )
{
	if (lhs.tv_sec == rhs.tv_sec)
		return lhs.tv_nsec < rhs.tv_nsec;
	else
		return lhs.tv_sec < rhs.tv_sec;
}

timespec operator -( const timespec &lhs, const timespec &rhs )
{
	timespec ret;

	ret.tv_nsec = lhs.tv_nsec - rhs.tv_nsec;
	ret.tv_sec = lhs.tv_sec - rhs.tv_sec;
	return ret;
}
