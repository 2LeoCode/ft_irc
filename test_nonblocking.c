/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_nonblocking.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Leo Suardi <lsuardi@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/01 15:46:57 by Leo Suardi        #+#    #+#             */
/*   Updated: 2022/05/02 21:28:47 by Leo Suardi       ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <poll.h>
#include <string.h>

int main(void) {
	char	buffer[2048];

	const int flags = fcntl(0, F_GETFL);
	fcntl(0, F_SETFL, flags | O_NONBLOCK);

	struct pollfd	pfd = { .events = POLLIN, .fd = 0, .revents = 0 };

	poll(&pfd, 1, -1);
	int ret = read(0, buffer, 100);
	buffer[ret] = 0;
	printf("%d\n%s\n%s\n", ret, buffer, strerror(errno));

	fcntl(0, F_SETFL, flags);
	return 0;
}
