/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exceptions.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Leo Suardi <lsuardi@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/21 14:06:13 by Leo Suardi        #+#    #+#             */
/*   Updated: 2022/03/24 00:29:56 by Leo Suardi       ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <exception>

#define EXCEPTION(NAME, WHAT) \
struct NAME : public std::exception {\
	const char	*what( void ) const throw ()\
	{\
		return WHAT;\
	}\
}

namespace data
{

	EXCEPTION( invalid_key, "trie: invalid key" );
	EXCEPTION( key_in_use, "trie: key already in use" );
	EXCEPTION( child_null, "trie: child is NULL" );

}
