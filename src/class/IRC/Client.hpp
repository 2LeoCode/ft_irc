/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Leo Suardi <lsuardi@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/16 00:53:55 by Leo Suardi        #+#    #+#             */
/*   Updated: 2022/03/16 00:55:25 by Leo Suardi       ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

namespace IRC {
	class Client {
		public:
			Client( void );
			~Client( void );


		private:
			int	m_sockfd;
	};
}
