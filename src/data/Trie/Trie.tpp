/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Trie.tpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsuardi <lsuardi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/15 21:08:38 by lsuardi           #+#    #+#             */
/*   Updated: 2022/05/13 14:35:51 by lsuardi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "../exceptions.hpp"
#include <cstring>

#define TEMPLATE template < class T, class Allocator >
#define TYPE Trie < T, Allocator >

#define CONSTRUCTOR TEMPLATE TYPE::Trie
#define DESTRUCTOR TEMPLATE TYPE::~Trie

#define MEMBER( RET_TYPE, NAME ) TEMPLATE RET_TYPE TYPE::NAME
#define NON_MEMBER( RET_TYPE, NAME ) TEMPLATE RET_TYPE NAME

namespace data
{

	CONSTRUCTOR( const Allocator &alloc )
	:	m_size( 
		0 ),
		m_root( alloc )
	{ }

	CONSTRUCTOR( const Trie &other )
	:	m_size( other.m_size ),
		m_root( other.m_root )
	{
		m_copy(&m_root, &other.m_root);
	}

	DESTRUCTOR( void )
	{
		clear();
	}


	// Operators

	MEMBER( TYPE&, operator = )( const Trie &other )
	{
		return clear().m_copy(&m_root, &other.m_root);
	}

	MEMBER( T&, operator [] )( const char *key )
	{
		try
		{
			return m_search(key);
		}
		catch (const std::exception &e)
		{ }
		return m_insert(key, T());
	}

	MEMBER( const T&, operator [] )( const char *key ) const
	{
		try
		{
			return m_search(key);
		}
		catch (const std::exception &e)
		{ }
		return m_insert(key, T());
	}


	// Accessors

	MEMBER( T&, at )( const char *key )
	{
		if (!key || !*key)
			throw invalid_key();
		return m_search((const unsigned char*)key);
	}
	
	MEMBER( const T&, at )( const char *key ) const
	{
		return m_search((const unsigned char*)key);
	}
	
	MEMBER( Allocator, get_allocator )( void ) const
	{
		return m_root.get_allocator();
	}


	// Capacity

	MEMBER( bool, empty )( void ) const
	{
		return !m_size;
	}
	
	MEMBER( typename TYPE::size_type, size )( void ) const
	{
		return m_size;
	}


	// Modifiers

	MEMBER( TYPE&, clear )( void )
	{
		for (int i = 0; i <= UCHAR_MAX; ++i)
			m_clear(m_root.children(i));
		m_root.clear();
		return *this;
	}

	MEMBER( TYPE&, insert )( const char *key, const T &value )
	{
		if (!key || !*key)
			throw invalid_key();
		return m_insert(&m_root, (const unsigned char*)key, value);
	}

	MEMBER( TYPE&, erase )( const char *key )
	{
		bool	deleted = false;

		if (!key || !*key)
			throw invalid_key();
		m_root.children(*key) = m_erase(
			m_root.children(*key), (const unsigned char*)key + 1, &deleted);
		if (!deleted)
			throw invalid_key();
		return *this;
	}

	MEMBER( TYPE&, swap )( Trie &other )
	{
		std::swap(m_alloc, other.m_alloc);
		std::swap(m_size, other.m_size);
		std::swap(m_root, other.m_root);
		return *this;
	}


	// Lookup

	MEMBER( bool, contains )( const char *key ) const
	{
		try
		{
			m_search(key);
		}
		catch (const std::exception &e)
		{
			return false;
		}
		return true;
	}


	// Private methods

	MEMBER( TYPE&, m_copy )( node_type *node, const node_type *other )
	{
		for (int i = 0; i <= UCHAR_MAX; ++i)
		{
			if (other->children(i))
			{
				node->children(i) = m_alloc.allocate(1);
				m_alloc.construct(node->children(i), *other->children(i));
				m_copy(node->children(i), other->children(i));
			}
		}
		return *this;
	}

	MEMBER( TYPE&, m_clear )( node_type *node )
	{
		if (node)
		{
			for (int i = 0; i <= UCHAR_MAX; ++i)
				m_clear(node->children(i));
			m_alloc.destroy(node);
			m_alloc.deallocate(node, 1);
		}
		return *this;
	}

	MEMBER( TYPE&, m_insert )( node_type *node, const unsigned char *key, const T &value)
	{
		while (*key)
		{
			if (!node->children(*key))
			{
				node->children(*key) = m_alloc.allocate(1);
				m_alloc.construct(node->children(*key), node_type());
			}
			node = node->children(*key++);
		}
		if (node->is_terminal())
			throw key_in_use();
		node->set_terminal_value(value);
		return *this;
	}

	MEMBER( typename TYPE::node_type*, m_erase )( node_type *node, const unsigned char *key, bool *deleted )
	{
		if (!node)
			return NULL;
		if (!*key)
		{
			if (node->is_terminal())
			{
				*deleted = true;
				if (!node->remove_terminal_value().has_children())
				{
					m_alloc.destroy(node);
					m_alloc.deallocate(node, 1);
					node = NULL;
				}
			}
			return node;
		}
		node->children(*key) = m_erase(node->children(*key), key + 1, deleted);
		if (*deleted && !node->is_terminal() && !node->has_children())
		{
			m_alloc.destroy(node);
			m_alloc.deallocate(node, 1);
			node = NULL;
		}
		return node;
	}

	MEMBER( T&, m_search )( const unsigned char *key )
	{
		node_type	*node = &m_root;

		while (*key)
		{
			if (!node->children(*key))
				throw invalid_key();
			node = node->children(*key++);
		}
		if (!node->is_terminal())
			throw invalid_key();
		return node->data();
	}

	MEMBER( const T&, m_search )( const unsigned char *key ) const
	{
		node_type	*node = &m_root;

		while (*key)
		{
			if (!node->children(*key))
				throw invalid_key();
			node = node->children(*key++);
		}
		if (!node->is_terminal())
			throw invalid_key();
		return node->data();
	}

	MEMBER( std::ostream&, m_display )( std::ostream &out, const node_type &node, const unsigned char *prefix ) const
	{
		const size_type	len = std::strlen((const char*)prefix);
		unsigned char	new_prefix[len + 2];

		if (node.is_terminal())
			out << "  \"" << prefix << "\" : " << node.data() << ",\n";
		new_prefix[len + 1] = 0;
		std::memcpy(new_prefix, prefix, len);
		for (int i = 0; i <= UCHAR_MAX; ++i)
		{
			if (node.children(i))
			{
				new_prefix[len] = i;
				m_display(out, node[i], new_prefix);
			}
		}
		return out;
	}


	// Non-member functions

	NON_MEMBER( std::ostream&, operator << )( std::ostream &out, const TYPE &t )
	{
		return out << "Trie\n{\n", t.m_display(out, t.m_root, (unsigned char*)"") << "}";
	}

}

#undef TEMPLATE
#undef TYPE
#undef CONSTRUCTOR
#undef DESTRUCTOR
#undef MEMBER
#undef NON_MEMBER
