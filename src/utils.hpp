/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Leo Suardi <lsuardi@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/13 11:27:09 by lsuardi           #+#    #+#             */
/*   Updated: 2022/06/08 18:13:28 by Leo Suardi       ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <netinet/in.h>
#include <string.h>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <ctime>
#include <cstdlib>

std::vector< std::string >	split( std::string s, char c );

std::string randomString( size_t size );


bool	operator <( const in6_addr&, const in6_addr& );
bool operator <( const timespec &lhs, const timespec &rhs );
timespec operator -( const timespec &lhs, const timespec &rhs );

template< class T >
std::ostream	&operator <<( std::ostream &out, const std::vector< T > &v )
{
	out << "size = " << v.size() << std::endl;
	for (typename std::vector<T>::const_iterator it = v.begin(); it != v.end(); ++it)
		out << '[' << *it << ']' << std::endl;
	return out;
}