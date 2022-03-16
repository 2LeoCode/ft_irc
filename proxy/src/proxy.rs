/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   proxy.rs                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Leo Suardi <lsuardi@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/16 15:48:40 by Leo Suardi        #+#    #+#             */
/*   Updated: 2022/03/16 18:19:58 by Leo Suardi       ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

pub mod net;

use std::result::Result;
use std::env::{args};

use crate::proxy::net::Addr;

pub fn	parse() -> Result<(Addr, Addr), ()>
{
	let	argv: Vec<String> = std::env::args().collect();

	if argv.len() != 3
	{
		()
	}
	let	addr: [Vec<String>; 2] = [
		argv[1]
			.split(|c| c == '.' || c == '/')
			.map(|s| s.to_string())
			.collect(),
		argv[2]
			.split(|c| c == '.' || c == '/')
			.map(|s| s.to_string())
			.collect()
	];

	Ok((
		Addr::new(
			[
				addr[0][0].parse().unwrap(),
				addr[0][1].parse().unwrap(),
				addr[0][2].parse().unwrap(),
				addr[0][3].parse().unwrap()
			] as [u8; 4],
			addr[0][4].parse().unwrap()
		),
		Addr::new(
			[
				addr[1][0].parse().unwrap(),
				addr[1][1].parse().unwrap(),
				addr[1][2].parse().unwrap(),
				addr[1][3].parse().unwrap()
			] as [u8; 4],
			addr[1][4].parse().unwrap()
		)
	))
}