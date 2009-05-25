#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>

#ifndef __WIN32__
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/select.h>
#else /* __WIN32__ */
#include <io.h>
#include <winsock2.h>
#endif /* __WIN32__ */

#include "server.h"

static struct timeval tv;
static fd_set readfds;
static fd_set writefds;
static int max_fd;

void select_restart()
{
#ifndef PUBLIC_SERVER
	tv.tv_sec = 0;
	tv.tv_usec = 0;
#else /* PUBLIC_SERVER */
	tv.tv_sec = 0;
	tv.tv_usec = 1000;
#endif /* PUBLIC_SERVER */

	FD_ZERO(&readfds);
	FD_ZERO(&writefds);
	max_fd = 0;
}

void select_add_sock_for_read(int sock)
{
	/*printf("select_add_sock_for_read %d\n", sock);*/

	FD_SET(sock, &readfds);

	if (sock > max_fd) {
		max_fd = sock;
	}
}

void select_add_sock_for_write(int sock)
{
	/*printf("select_add_sock_for_write %d\n", sock);*/

	FD_SET(sock, &writefds);

	if (sock > max_fd) {
		max_fd = sock;
	}
}

int select_action()
{
	int ret;

#ifndef PUBLIC_SERVER
	ret = select(max_fd + 1, &readfds, &writefds, (fd_set *) NULL, &tv);
#else /* PUBLIC_SERVER */
	list_t *listClient;
	listClient = server_get_list_clients();

	if (listClient->count == 0) {
		ret = select(max_fd + 1, &readfds, &writefds, (fd_set *) NULL, NULL);
		server_set_time();
	} else {
		ret = select(max_fd + 1, &readfds, &writefds, (fd_set *) NULL, &tv);
	}
#endif /* PUBLIC_SERVER */

	return ret;
}

int select_is_change_sock_for_read(int sock)
{
	int ret;

	ret = FD_ISSET(sock, &readfds);

	/*printf("select_is_change_sock %d -> %d\n", sock, ret);*/

	return ret;
}

int select_is_change_sock_for_write(int sock)
{
	int ret;

	ret = FD_ISSET(sock, &writefds);

	/*printf("select_is_change_sock %d -> %d\n", sock, ret);*/

	return ret;
}
