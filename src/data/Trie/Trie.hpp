/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Trie.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Leo Suardi <lsuardi@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/15 21:05:00 by lsuardi           #+#    #+#             */
/*   Updated: 2022/03/24 00:41:25 by Leo Suardi       ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "../Trienode/Trienode.hpp"
#include <cstddef>
#include <iostream>

namespace data
{

	template < class T, class Allocator = std::allocator< T > >
	class Trie
	{
		public:
			typedef Allocator	allocator_type;
			typedef T			value_type;
			typedef std::size_t	size_type;

			Trie( const Allocator& = Allocator() );
			Trie( const Trie& );
			~Trie( void );

			// Operators
			Trie	&operator =( const Trie& );
			T		&operator []( const char* );
			const T	&operator []( const char* ) const;

			// Accessors
			T			&at( const char* );
			const T		&at( const char* ) const;
			Allocator	get_allocator( void ) const;

			// Capacity
			bool		empty( void ) const;
			size_type	size( void ) const;


			// Modifiers
			Trie	&clear( void );
			Trie	&insert( const char*, const T& );
			Trie	&erase( const char* );
			Trie	&swap( Trie& );

			// Lookup
			bool	contains( const char* ) const;

			// Non-member functions
			template < class _T, class _Allocator >
			friend std::ostream	&operator <<( std::ostream&, const Trie< _T, _Allocator >& );


		private:
			typedef Trienode< T, Allocator >								node_type;
			typedef typename Allocator::template rebind< node_type >::other	node_allocator_type;

			// Private methods
			Trie			&m_copy( node_type*, const node_type* );
			Trie			&m_clear( node_type* );
			Trie			&m_insert( node_type*, const unsigned char*, const T& );
			node_type		*m_erase( node_type*, const unsigned char*, bool* );
			T				&m_search( const unsigned char* );
			const T			&m_search( const unsigned char* ) const;
			std::ostream	&m_display( std::ostream&, const node_type&, const unsigned char* ) const;

			node_allocator_type	m_alloc;
			size_type			m_size;
			node_type			m_root;
	};

}

#include "Trie.tpp"

// For clients
//
// A name can only contain the following characters:
// 
// A through to Z. (Lowercase and uppercase.)
// 0 through to 9.
// `|^_-{}[] and (backslash)
// 
// And a name cannot start with a number or hyphen.
//
// Note that certain networks have specific requirements for nicknames that
// other networks don't have. Also, some networks allow unicode nicknames.


// A channel is a named group of one or more clients which will all
// receive messages addressed to that channel.  The channel is created
// implicitly when the first client joins it, and the channel ceases to
// exist when the last client leaves it.  While channel exists, any
// client can reference the channel using the name of the channel.
// 
// Channels names are strings (beginning with a '&' or '#' character) of
// length up to 200 characters.  Apart from the the requirement that the
// first character being either '&' or '#'; the only restriction on a
// channel name is that it may not contain any spaces (' '), a control G
// (^G or ASCII 7), or a comma (',' which is used as a list item
// separator by the protocol).
// 
// There are two types of channels allowed by this protocol.  One is a
// distributed channel which is known to all the servers that are
// connected to the network. These channels are marked by the first
// character being a only clients on the server where it exists may join
// it.  These are distinguished by a leading '&' character.  On top of
// these two types, there are the various channel modes available to
// alter the characteristics of individual channels.  See section 4.2.3
// (MODE command) for more details on this.
