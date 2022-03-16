/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Trienode.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsuardi <lsuardi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/15 23:34:51 by Leo Suardi        #+#    #+#             */
/*   Updated: 2022/03/16 19:13:19 by lsuardi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <climits>
#include <memory>

template < class T, class Allocator >
class Trienode {
	public:
		Trienode( const Allocator& = std::allocator< T > );
		Trienode( const Trienode& );
		Trienode( T&, const Allocator& = std::allocator< T > );
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
		Allocator		get_allocator( void ) const;

		// Lookup
		bool	has_children( void ) const;
		bool	is_terminal( void ) const;


	private:
		Trienode	*m_children[UCHAR_MAX + 1];
		Allocator	m_alloc;
		T			*m_data;
};
