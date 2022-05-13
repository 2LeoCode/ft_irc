/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Trienode.tpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsuardi <lsuardi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/16 00:03:27 by Leo Suardi        #+#    #+#             */
/*   Updated: 2022/05/13 13:13:54 by lsuardi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "../exceptions.hpp"
#include <cstddef>
#include <cstring>
#include <stdexcept>

#define TEMPLATE template < class T, class Allocator >
#define TYPE Trienode < T, Allocator >

#define MEMBER( RET_TYPE, NAME ) TEMPLATE RET_TYPE TYPE::NAME
#define CONSTRUCTOR TEMPLATE TYPE::Trienode
#define DESTRUCTOR TEMPLATE TYPE::~Trienode

namespace data
{

	CONSTRUCTOR( const Allocator &alloc )
	:	m_alloc( alloc ),
		m_data( NULL )
	{
		clear();
	}

	CONSTRUCTOR( const Trienode &other )
	:	m_alloc( other.m_alloc ),
		m_data( NULL )
	{
		clear();
		if (other.m_data)
		{
			m_data = m_alloc.allocate(1);
			m_alloc.construct(m_data, *other.m_data);
		}
	}

	CONSTRUCTOR( T &value, const Allocator &alloc )
	:	m_alloc( alloc ),
		m_data( m_alloc.allocate(1) )
	{
		clear();
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

	MEMBER( TYPE&, operator = )( const Trienode &other )
	{
		if (m_data)
		{
			m_alloc.destroy(m_data);
			m_alloc.deallocate(m_data, 1);
			m_data = NULL;
		}
		m_alloc = other.m_alloc;
		if (other.m_data)
		{
			m_data = m_alloc.allocate(1);
			m_alloc.construct(m_data, *other.m_data);
		}
	}

	MEMBER( TYPE&, operator [] )( unsigned char pos )
	{
		if (!m_children[pos])
			throw child_null();
		return *m_children[pos];
	}

	MEMBER( const TYPE&, operator [] )( unsigned char pos ) const
	{
		if (!m_children[pos])
			throw child_null();
		return *m_children[pos];
	}


	// Accessors

	MEMBER( T&, data )( void )
	{
		return *m_data;
	}

	MEMBER( const T&, data )( void ) const
	{
		return *m_data;
	}

	MEMBER( TYPE*&, children )( unsigned char pos )
	{
		return m_children[pos];
	}

	MEMBER( const TYPE*&, children )( unsigned char pos ) const
	{
		return const_cast< const TYPE*& >(m_children[pos]);
	}

	MEMBER( Allocator, get_allocator )( void ) const
	{
		return m_alloc;
	}

	// Modifiers

	MEMBER( TYPE&, clear )( void )
	{
		std::memset(m_children, 0, sizeof(m_children));
		return *this;
	}

	MEMBER( TYPE&, set_terminal_value )( const T &value )
	{
		if (m_data)
		{
			m_alloc.destroy(m_data);
			m_alloc.deallocate(m_data, 1);
		}
		m_data = m_alloc.allocate(1);
		m_alloc.construct(m_data, value);
		return *this;
	}

	MEMBER( TYPE&, remove_terminal_value )( void )
	{
		if (m_data)
		{
			m_alloc.destroy(m_data);
			m_alloc.deallocate(m_data, 1);
			m_data = NULL;
		}
		return *this;
	}

	MEMBER( TYPE&, set_allocator )( const Allocator &alloc )
	{
		m_alloc = alloc;
		return *this;
	}


	// Lookup

	MEMBER( bool, has_children )( void ) const
	{
		for (int i = 0; i < UCHAR_MAX; ++i)
			if (m_children[i])
				return true;
		return false;
	}

	MEMBER( bool, is_terminal )( void ) const
	{
		return m_data;
	}

}

#undef TEMPLATE
#undef TYPE
#undef CONSTRUCTOR
#undef DESTRUCTOR
#undef MEMBER
#undef NON_MEMBER
