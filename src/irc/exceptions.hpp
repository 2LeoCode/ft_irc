/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exceptions.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsuardi <lsuardi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/08 14:31:16 by lsuardi           #+#    #+#             */
/*   Updated: 2022/04/08 15:18:26 by lsuardi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "../exception_macro.hpp"

namespace irc
{

	EXCEPTION( socket_failure, "irc: failed to open server socket" );
	EXCEPTION( socket_already_bound, "irc: socket is already bound" );
	EXCEPTION( uninitialized_socket, "irc: server socket is uninitialized" );

}
