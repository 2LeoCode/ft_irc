/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Trienode.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Leo Suardi <lsuardi@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/16 00:03:27 by Leo Suardi        #+#    #+#             */
/*   Updated: 2022/03/16 00:53:02 by Leo Suardi       ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Trienode.hpp"

#define TEMPLATE template < class T, class Allocator >
#define TYPE Trienode < T, Allocator >

#define MEMBER( RET_TYPE, NAME ) TEMPLATE RET_TYPE TYPE::NAME
#define CONSTRUCTOR TEMPLATE TYPE::Trienode
#define DESTRUCTOR TEMPLATE TYPE::~Trienode

static char	*g_printable =
	" !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMN"
	"OPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";

CONSTRUCTOR( void )
{
}

CONSTRUCTOR( const std::string )
{
}

CONSTRUCTOR( const Trienode& )
{
}

CONSTRUCTOR( T& )
{
}

DESTRUCTOR( void )
{
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

MEMBER( bool, has_children )( void ) const
{
}

MEMBER( bool, is_terminal )( void ) const
{
	return m_data;
}
