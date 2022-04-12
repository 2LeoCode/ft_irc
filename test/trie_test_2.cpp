/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   trie_test_2.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsuardi <lsuardi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/24 00:43:13 by Leo Suardi        #+#    #+#             */
/*   Updated: 2022/04/12 14:47:24 by lsuardi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../src/data/Trie/Trie.hpp"
#include <fstream>
#include <string>
#include <stdlib.h>

int	trie_test_2( void )
{
	data::Trie< int >	t;
	std::ifstream		in("test/words.txt");
	std::string			word;

	if (!in)
	{
		std::cerr << "Error while opening file" << std::endl;
		return 1;
	}
	for (int i = 0; i < 466552; ++i)
	{
		std::getline(in, word);
		t.insert(word.data(), random() % 100000);
	}
	std::cout << t << std::endl;
	std::cout << "accessing to a single element" << std::endl;
	std::cout << "CAT: " << t.at("CAT") << std::endl;
	std::cout << "BIG: " << t.at("hjdiwehdiouwogwouidhiwjdopwjdopwjdpowjdpowjdiopjwdopjqipfjqopefopqwjfopqhfpiqhjdiwehdiouwogwouidhiwjdopwjdopwjdpowjdpowjdiopjwdopjqipfjqopefopqwjfopqhfpiqhjdiwehdiouwogwouidhiwjdopwjdopwjdpowjdpowjdiopjwdopjqipfjqopefopqwjfopqhfpiqhjdiwehdiouwogwouidhiwjdopwjdopwjdpowjdpowjdiopjwdopjqipfjqopefopqwjfopqhfpiqhjdiwehdiouwogwouidhiwjdopwjdopwjdpowjdpowjdiopjwdopjqipfjqopefopqwjfopqhfpiqhjdiwehdiouwogwouidhiwjdopwjdopwjdpowjdpowjdiopjwdopjqipfjqopefopqwjfopqhfpiq") << std::endl;
	std::cout << "done" << std::endl;
	return 0;
}
