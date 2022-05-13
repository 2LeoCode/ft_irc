/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsuardi <lsuardi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/13 11:28:37 by lsuardi           #+#    #+#             */
/*   Updated: 2022/05/13 11:32:40 by lsuardi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils.hpp"

bool	operator <( const in6_addr &lhs, const in6_addr &rhs )
{
	return memcmp(&lhs, &rhs, sizeof(lhs)) < 0;
}
