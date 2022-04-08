/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   trie_test_1.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Leo Suardi <lsuardi@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/24 00:42:17 by Leo Suardi        #+#    #+#             */
/*   Updated: 2022/03/27 16:37:06 by Leo Suardi       ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../src/data/Trie/Trie.hpp"

int	trie_test_1( void )
{
	data::Trie< int >	t;

	t.insert("HELLO", 1)
		.insert("SALUT", 2)
		.insert("HELL", 3)
		.insert("SALUTE", 4)
		.insert("CONICHIWA", 5)
		.insert("CON DE TA MERE", 6)
		.insert("CON DE TES MORTS", 7)
		.insert("HELLO WORLD", 8);
	try
	{
		t.insert("SALUT", 9);
	}
	catch (const std::exception &e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
	
		std::cout << t << std::endl;
		t.erase("HELL")
			.erase("SALUTE")
			.erase("CON DE TES MORTS");
		try
		{
			t.erase("HELL");
		}
		catch (const std::exception &e)
		{
			std::cerr << "Error: " << e.what() << std::endl;
			std::cout << t << std::endl;
			try
			{
				(void)t.at("CONICHIWA");
			}
			catch (...)
			{
				goto KO;
			}
			t.erase("CON DE TA MERE")
				.erase("CONICHIWA")
				.erase("HELLO")
				.erase("HELLO WORLD")
				.erase("SALUT");
			try
			{
				t.insert("", 1);
			}
			catch (const std::exception &e)
			{
				std::cerr << "Error: " << e.what() << std::endl;
				t.insert("koeiohfq", 1)
					.insert("wokhdwudgwd", 1)
					.insert("jd82y7d,pw", 1)
					.insert("lwpei02", 42)
					.insert("wjd9hd8i2k02j9n2du9h2nu2h8snj", 1)
					.insert("230u9mwdw", 1);
				std::cout << "search: " << t.at("lwpei02") << std::endl;
				try
				{
					(void)t.at("jd82y7d,pwd");
				}
				catch (const std::exception &e)
				{
					std::cerr << "Error: " << e.what() << std::endl;
					try
					{
						(void)t.at("jd82y7d,p");
					}
					catch (const std::exception &e)
					{
						std::cerr << "Error: " << e.what() << std::endl;
						std::cout << t << "\n" << t.clear() << std::endl;
						return 0;
					}
				}
			}
		}
	}
	KO:
	std::cerr << "KO" << std::endl;
	return 1;
}
