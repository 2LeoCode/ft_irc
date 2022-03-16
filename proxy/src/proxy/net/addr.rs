/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   addr.rs                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Leo Suardi <lsuardi@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/16 15:48:32 by Leo Suardi        #+#    #+#             */
/*   Updated: 2022/03/16 17:40:24 by Leo Suardi       ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#[derive(Debug)]
pub struct Addr {
	ip: [u8; 4],
	port: u16
}

impl Addr
{
	pub fn	new(ip: [u8; 4], port: u16) -> Addr
	{
		Addr {
			ip,
			port
		}
	}

	pub	fn	ip(&self) -> [u8; 4]
	{
		self.ip
	}

	pub fn	port(&self) -> u16
	{
		self.port
	}
}
