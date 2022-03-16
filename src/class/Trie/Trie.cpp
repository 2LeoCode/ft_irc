/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Trie.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Leo Suardi <lsuardi@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/15 21:08:38 by lsuardi           #+#    #+#             */
/*   Updated: 2022/03/16 00:10:28 by Leo Suardi       ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Trie.hpp"

#define TEMPLATE template < class T, class Allocator >
#define TYPE Trie < T, Allocator >

#define MEMBER( RET_TYPE, NAME ) TEMPLATE RET_TYPE TYPE::NAME
#define CONSTRUCTOR TEMPLATE TYPE::Trie
#define DESTRUCTOR TEMPLATE TYPE::~Trie

CONSTRUCTOR( void )
{
}

CONSTRUCTOR( const Trie &other )
{
}

DESTRUCTOR( void )
{
}


// Operators

MEMBER( TYPE&, operator = )( const Trie &other )
{
}

MEMBER( T&, operator [] )( const char *key )
{
}

MEMBER( const T&, operator [] )( const char *key ) const
{
}


// Accessors

MEMBER( T&, at )( const char *key )
{
}

MEMBER( const T&, at )( const char *key ) const
{
}


// Capacity

MEMBER( bool, empty )( void ) const
{
	return !m_size;
}

MEMBER( TYPE::size_type, size )( void ) const
{
	return m_size;
}


// Modifiers

MEMBER( void, clear )( void )
{
}

MEMBER( void, insert )( const char *key, T &value )
{
}

MEMBER( void, erase )( const char *key )
{
}

MEMBER( void, swap )( Trie &other )
{
	std::swap(m_alloc, other.m_alloc);
	std::swap(m_size, other.m_size);
	std::swap(m_data, other.m_data);
}


// Lookup

MEMBER( TYPE::node_type*, find )( const char *key )
{
}

MEMBER( TYPE::node_type const*, find )( const char *key ) const
{
}

MEMBER( bool, contains )( const char *key ) const
{
}
