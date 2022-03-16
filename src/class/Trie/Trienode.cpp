/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Trienode.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsuardi <lsuardi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/16 00:03:27 by Leo Suardi        #+#    #+#             */
/*   Updated: 2022/03/16 19:13:32 by lsuardi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Trienode.hpp"

#include <cstddef>
#include <cstring>

#define TEMPLATE template < class T, class Allocator >
#define TYPE Trienode < T, Allocator >

#define MEMBER( RET_TYPE, NAME ) TEMPLATE RET_TYPE TYPE::NAME
#define CONSTRUCTOR TEMPLATE TYPE::Trienode
#define DESTRUCTOR TEMPLATE TYPE::~Trienode


CONSTRUCTOR( const Allocator &alloc )
:	m_alloc(alloc),
	m_data(NULL)
{
	std::memset(m_children, 0, sizeof(m_children) / sizeof(*m_children));
}

CONSTRUCTOR( const Trienode &other )
:	m_alloc(other.m_alloc),
	m_data(NULL)
{
	std::memset(m_children, 0, sizeof(m_children) / sizeof(*m_children));
	if (other.m_data)
	{
		m_data = m_alloc.allocate(1);
		m_alloc.construct(m_data, *other.m_data);
	}
}

CONSTRUCTOR( T &value, const Allocator &alloc )
:	m_alloc(alloc),
	m_data(m_alloc.allocate(1))
{
	std::memset(m_children, 0, sizeof(m_children) / sizeof(*m_children));
	m_alloc.construct(m_data, value);
}

DESTRUCTOR( void )
{
	if (m_data)
	{
		m_alloc.destroy(m_data);
		m_alloc.deallocate(m_data, 1);
	}
}


// Operators

MEMBER( TYPE&, operator = )( const Trienode& )
{
}

MEMBER( TYPE*&, operator [] )( unsigned char )
{
}

MEMBER( const TYPE*&, operator [] )( unsigned char ) const
{
}


// Accessors

MEMBER( T*, data )( void )
{
	return m_data;
}

MEMBER( const T*, data )( void ) const
{
	return m_data;
}

MEMBER( TYPE*&, children )( unsigned char )
{
}

MEMBER( const TYPE*&, children )( unsigned char ) const
{
}


// Lookup

MEMBER( bool, has_children )( void ) const
{
}

MEMBER( bool, is_terminal )( void ) const
{
	return m_data;
}
