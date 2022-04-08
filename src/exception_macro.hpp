/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exception_macro.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsuardi <lsuardi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/08 14:28:25 by lsuardi           #+#    #+#             */
/*   Updated: 2022/04/08 14:28:41 by lsuardi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#define EXCEPTION(NAME, WHAT) \
struct NAME : public std::exception {\
	const char	*what( void ) const throw ()\
	{\
		return WHAT;\
	}\
}
