/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Trienode.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Leo Suardi <lsuardi@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/15 23:34:51 by Leo Suardi        #+#    #+#             */
/*   Updated: 2022/03/16 00:46:20 by Leo Suardi       ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>

template < class T, class Allocator >
class Trienode {
	public:
		Trienode( void );
		Trienode( const std::string );
		Trienode( const Trienode& );
		Trienode( T& );
		~Trienode( void );

		// Operators
		Trienode		&operator =( const Trienode& );
		Trienode		*&operator []( unsigned char );
		const Trienode	*&operator []( unsigned char ) const;

		// Accessors
		T				*data( void );
		const T			*data( void ) const;
		Trienode		*&children( unsigned char );
		const Trienode	*&children( unsigned char ) const;

		// Lookup
		bool	has_children( void ) const;
		bool	is_terminal( void ) const;


	private:
		const std::string	m_charset;
		Trienode			**m_children;
		Allocator			m_alloc;
		T					*m_data;
};
