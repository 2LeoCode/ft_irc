/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.rs                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsuardi <lsuardi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/16 10:22:51 by Leo Suardi        #+#    #+#             */
/*   Updated: 2022/05/11 16:33:51 by lsuardi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

use async_std::prelude::*;
use std::thread;
use async_std::net::{TcpListener/*, TcpStream, Shutdown */};
use std::io::{/* Read, Write, */Error, ErrorKind::InvalidInput};
use std::result::Result;
use std::env;
use std::sync::mpsc;
use seq_macro::seq;

fn	proxy_bind(ip1: &str, ip2: &str) -> Result<[TcpListener; 2], Error>
{
	Ok([TcpListener::bind(ip1)?, TcpListener::bind(ip2)?])
}

async fn handle_connection(mut stream: TcpStream) {
    let mut buffer = [0; 1024];
    stream.read(&mut buffer).await.unwrap();

    //<-- snip -->
    stream.write(response.as_bytes()).await.unwrap();
    stream.flush().await.unwrap();
}

#[async_std::main]
async fn	main() -> Result<(), Error>
{
	let argv: Vec<String> = env::args().collect();

	if argv.len() != 3
	{
		return Err(Error::new(InvalidInput, format!("usage: {} <ip1:port1> <ip2:port2>", argv[0])))
	}
	let listener = match proxy_bind(&argv[1], &argv[2]) {
		Ok(v) =>
		{
			let chan = seq!(N in 0..2 {(#(mpsc::channel(),)*)});
			let handle = thread::spawn(move || {
				let mut buffer = [0; 1024];
				for stream in v[0].incoming()
				while true
				{
   					stream.read(&mut buffer).await.unwrap();
					chan.0.0.send(buffer).unwrap();
					let received = chan.1.1.recv().unwrap();
					
    				v.write(response.as_bytes()).await.unwrap();
    				stream.flush().await.unwrap();
				}
				// chan.0.0.send("lol").unwrap();
				// let received = chan.1.1.recv().unwrap();
				// println!("received back message: {}", received);
			});
			while true
			{
				
			}
			//let received = chan.0.1.recv().unwrap();
			//println!("received message: {}", received);
			//chan.1.0.send("mdr").unwrap();
			//handle.join().unwrap();
		}
		Err(e) => return Err(Error::new(e.kind(), "Failed to connect"))
	};
	Ok(())
}