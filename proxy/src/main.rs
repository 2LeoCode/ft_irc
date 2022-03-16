/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.rs                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Leo Suardi <lsuardi@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/16 10:22:51 by Leo Suardi        #+#    #+#             */
/*   Updated: 2022/03/16 18:35:31 by Leo Suardi       ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

mod proxy;

use proxy::net::Addr;

fn	main()
{
	let	addr: (Addr, Addr) = proxy::parse().unwrap();

	println!("addr.1 = {:?}\naddr.2 = {:?}", addr.0, addr.1);
}
