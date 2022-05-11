/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exceptions.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsuardi <lsuardi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/21 14:06:13 by Leo Suardi        #+#    #+#             */
/*   Updated: 2022/04/08 14:28:46 by lsuardi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <exception>
#include "../exception_macro.hpp"

namespace data
{

	EXCEPTION( invalid_key, "trie: invalid key" );
	EXCEPTION( key_in_use, "trie: key already in use" );
	EXCEPTION( child_null, "trie: child is NULL" );

}
