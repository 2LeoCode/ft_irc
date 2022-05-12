/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   proxy.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsuardi <lsuardi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/11 16:34:44 by lsuardi           #+#    #+#             */
/*   Updated: 2022/05/12 10:58:48 by lsuardi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <arpa/inet.h>
#include <netinet/in.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/poll.h>

#include <errno.h>
#include <limits.h>
#include <fcntl.h>
#include <unistd.h>

#include <vector>
#include <algorithm>

#define BUFSIZE 2048

static char *g_exec;

void	err_n_die( void ) {
	fprintf(stderr, "usage: %s <ip> <server_port> <proxy_port>\n", g_exec);
	exit(EXIT_FAILURE);
}

short	to_short( char *s ) {
	errno = 0;
	long n = strtol(s, &s, 0);
	if (errno == ERANGE || *s || n < 0 || n > SHRT_MAX)
		err_n_die();
	return n;
}

void	set_nonblock( int fd ) {
	const int flags = fcntl(fd, F_GETFL);
	fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

template < class T >
std::vector< T >	operator+(const std::vector<T>&lhs, const std::vector<T>&rhs) {
	std::vector< T >	ret;
	ret.reserve(lhs.size() + rhs.size());

	ret.insert(ret.end(), lhs.begin(), lhs.end());
	ret.insert(ret.end(), rhs.begin(), rhs.end());
	return ret;
}

int		main( int argc, char **argv ) {
	g_exec = argv[0];

	if (argc != 4) err_n_die();


	short serv_p, proxy_p;
	sockaddr_in serv_addr = { 0 }, proxy_addr = { 0 };
	std::vector< pollfd > gfds;
	std::vector< int >	lfds, sfds;
	int proxy_fd;
	char buf[BUFSIZE + 1];

	serv_p = to_short(argv[2]);
	proxy_p = to_short(argv[3]);

	proxy_fd = socket(AF_INET, SOCK_STREAM, 0);

	set_nonblock(proxy_fd);

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(serv_p);
	inet_pton(AF_INET, argv[1], &serv_addr.sin_addr);

	proxy_addr.sin_family = AF_INET;
	proxy_addr.sin_port = htons(proxy_p);
	proxy_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	bind(proxy_fd, (sockaddr*)&proxy_addr, sizeof(proxy_addr));
	listen(proxy_fd, 32);

	gfds.push_back((pollfd){proxy_fd, POLLIN, 0});

	printf("Waiting for connection on port %hd\n", proxy_p);
	while (true) {
		int ret = poll(gfds.data(), gfds.size(), -1);
		if (gfds[0].revents == POLLIN) {
			int	new_fd = accept(proxy_fd, NULL, NULL);
			gfds.push_back((pollfd){new_fd, POLLIN, 0});
			lfds.push_back(new_fd);

			int	serv_fd = socket(AF_INET, SOCK_STREAM, 0);
			set_nonblock(serv_fd);
			connect(serv_fd, (sockaddr*)&serv_addr, sizeof(serv_addr));
			gfds.push_back((pollfd){serv_fd, POLLIN, 0});
			sfds.push_back(serv_fd);
		}
		for (int i = 1; i < gfds.size(); ++i) {
			if (gfds[i].revents == POLLIN) {
				int	rret = recv(gfds[i].fd, buf, BUFSIZE, 0);
				if (rret < 0) {
					if (errno != EWOULDBLOCK) {
						perror("recv");
						exit(EXIT_FAILURE);
					}
				} else if (rret == 0) {
					auto id = std::find(lfds.begin(), lfds.end(), gfds[i].fd) - lfds.begin();
					printf("Client %zu disconnected\n", id);
					close(lfds[id]);
					close(sfds[id]);
					gfds.erase(gfds.begin() + i);
					lfds.erase(lfds.begin() + id);
					sfds.erase(sfds.begin() + id);
				} else {
					buf[rret] = 0;

					auto lit = std::find(lfds.begin(), lfds.end(), gfds[i].fd);
					auto sit = std::find(sfds.begin(), sfds.end(), gfds[i].fd);
					ptrdiff_t id;

					if (lit != lfds.end()) {
						id = lit - lfds.begin();
						printf("Client %zu sent %d bytes to server:\n\n%s\n", id, rret, buf);
						send(sfds[id], buf, rret, 0);
					} else {
						id = sit - sfds.begin();
						printf("Server sent %d bytes to client %zu:\n\n%s\n", rret, id, buf);
						send(lfds[id], buf, rret, 0);
					}
				}
			}
		}
	}
	return 0;
}
