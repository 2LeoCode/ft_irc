/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsuardi <lsuardi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/13 11:27:09 by lsuardi           #+#    #+#             */
/*   Updated: 2022/05/13 17:16:27 by lsuardi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <netinet/in.h>
#include <string.h>
#include <iostream>
#include <vector>

bool	operator <( const in6_addr&, const in6_addr& );

template< class T >
std::ostream	&operator <<( std::ostream &out, const std::vector< T > &v )
{
	out << "size = " << v.size() << std::endl;
	for (typename std::vector<T>::const_iterator it = v.begin(); it != v.end(); ++it)
		out << '[' << *it << ']' << std::endl;
	return out;
}
