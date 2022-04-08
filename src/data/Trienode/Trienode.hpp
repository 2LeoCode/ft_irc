/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Trienode.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Leo Suardi <lsuardi@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/15 23:34:51 by Leo Suardi        #+#    #+#             */
/*   Updated: 2022/03/24 00:13:09 by Leo Suardi       ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <climits>
#include <memory>

namespace data
{

	template < class T, class Allocator = std::allocator< T > >
	class Trienode {
		public:
			Trienode( const Allocator& = Allocator() );
			Trienode( const Trienode& );
			Trienode( T&, const Allocator& = Allocator() );
			~Trienode( void );

			// Operators
			Trienode		&operator =( const Trienode& );
			Trienode		&operator []( unsigned char );
			const Trienode	&operator []( unsigned char ) const;

			// Accessors
			T				&data( void );
			const T			&data( void ) const;
			Trienode		*&children( unsigned char );
			const Trienode	*&children( unsigned char ) const;
			Allocator		get_allocator( void ) const;

			// Modifiers
			Trienode	&clear( void );
			Trienode	&set_terminal_value( const T& );
			Trienode	&remove_terminal_value( void );
			Trienode	&set_allocator( const Allocator& );

			// Lookup
			bool	has_children( void ) const;
			bool	is_terminal( void ) const;


		private:
			Trienode	*m_children[UCHAR_MAX + 1];
			Allocator	m_alloc;
			T			*m_data;
	};

}

#include "Trienode.tpp"
