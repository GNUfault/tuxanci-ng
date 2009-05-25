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

#include "main.h"
#include "list.h"
#include "tux.h"
#include "proto.h"
#include "server.h"
#include "myTimer.h"
#include "arena.h"
#include "net_multiplayer.h"
#include "checkFront.h"
#include "protect.h"
#include "index.h"

#ifndef PUBLIC_SERVER
#include "world.h"
#else /* PUBLIC_SERVER */
#include "publicServer.h"
#include "highScore.h"
#include "log.h"
#endif /* PUBLIC_SERVER */

#include "udp.h"
#include "udp_server.h"
#include "serverSelect.h"

static sock_udp_t *sock_server_udp;
static sock_udp_t *sock_server_udp_second;

client_t *server_udp_new_client(sock_udp_t *sock_udp)
{
	client_t *new;

	assert(sock_udp != NULL);

	new = server_new_any_client();
	new->type = CLIENT_TYPE_UDP;
	new->socket_udp = sock_udp;

#ifdef PUBLIC_SERVER
	char str_log[STR_LOG_SIZE];
	char str_ip[STR_IP_SIZE];

	sock_udp_get_ip(sock_udp, str_ip, STR_IP_SIZE);
	sprintf(str_log, _("New player [%s]:%d connected"), str_ip, sock_udp_get_port(sock_udp));
	log_add(LOG_INF, str_log);
#endif /* PUBLIC_SERVER */

	return new;
}

void server_udp_destroy_client(client_t *p)
{
	check_front_event(p);

#ifdef PUBLIC_SERVER
	char str_log[STR_LOG_SIZE];
	char str_ip[STR_IP_SIZE];

	sock_udp_get_ip(p->socket_udp, str_ip, STR_IP_SIZE);
	sprintf(str_log, _("Player [%s]:%d disconnected"), str_ip, sock_udp_get_port(p->socket_udp));
	log_add(LOG_INF, str_log);
#endif /* PUBLIC_SERVER */

	sock_udp_destroy(p->socket_udp);

	server_destroy_any_client(p);
}

int server_udp_init(char *ip4, int port4, char *ip6, int port6)
{
	int ret;

	ret = 0;

	if (ip4 != NULL) {
		sock_server_udp = sock_udp_bind(ip4, port4);

		if (sock_server_udp != NULL) {
			ret++;
			DEBUG_MSG(_("[Debug] Starting server on [%s]:%d\n"), ip4, port4);
		} else {
			DEBUG_MSG(_("[Error] Unable to start server on [%s]:%d\n"), ip4, port4);
		}
	}

	if (ip6 != NULL) {
		sock_server_udp_second = sock_udp_bind(ip6, port6);

		if (sock_server_udp_second != NULL) {
			ret++;
			DEBUG_MSG(_("[Debug] Starting server on [%s]:%d\n"), ip6, port6);
		} else {
			DEBUG_MSG(_("[Error] Unable to start server on [%s]:%d\n"), ip6, port6);
		}
	}

	return ret;
}

static void eventCreateNewUdpClient(sock_udp_t *socket_udp)
{
	list_t *listClient;
	client_t *client;

	assert(socket_udp != NULL);

	listClient = server_get_list_clients();

	client = server_udp_new_client(socket_udp);
	list_add(listClient, client);
}

static client_t *findUdpClient(sock_udp_t *sock_udp)
{
	list_t *listClient;
	int port;
	int i;

	port = sock_udp_get_port(sock_udp);
	listClient = server_get_list_clients();

	for (i = 0; i < listClient->count; i++) {
		client_t *client;

		client = (client_t *) listClient->list[i];

		if (client->type == CLIENT_TYPE_UDP &&
		    sock_udp_get_port(client->socket_udp) == port) {
			return client;
		}
	}

	return NULL;
}

static void client_eventUdpSelect(sock_udp_t *sock_server)
{
	sock_udp_t *sock_client;
	client_t *client;
	char listRecvMsg[STR_SIZE];
	bool_t isCreateNewClient;
	int ret;

	assert(sock_server != NULL);

	sock_client = sock_udp_new();
	isCreateNewClient = FALSE;

	memset(listRecvMsg, 0, STR_SIZE);

	ret = sock_udp_read(sock_server, sock_client, listRecvMsg, STR_SIZE - 1);

	client = findUdpClient(sock_client);

	if (client == NULL) {
		eventCreateNewUdpClient(sock_client);
		client = findUdpClient(sock_client);
		isCreateNewClient = TRUE;
	}

	if (client == NULL) {
		fprintf(stderr, _("[Error] UDP client not found\n"));
		return;
	}

	if (isCreateNewClient == FALSE) {
		sock_udp_destroy(sock_client);
	}

	if (ret <= 0) {
		client->status = NET_STATUS_ZOMBIE;
		return;
	}

	/*printf("add packet >>%s<<\n", listRecvMsg);*/
	list_add(client->listRecvMsg, strdup(listRecvMsg));
}

void server_udp_set_select()
{
	if (sock_server_udp != NULL) {
		select_add_sock_for_read(sock_server_udp->sock);
	}

	if (sock_server_udp_second != NULL) {
		select_add_sock_for_read(sock_server_udp_second->sock);
	}
}

int server_udp_select_sock()
{
	int count;

	count = 0;

	if (sock_server_udp != NULL) {
		if (select_is_change_sock_for_read(sock_server_udp->sock)) {
			client_eventUdpSelect(sock_server_udp);
			count++;
		}
	}

	if (sock_server_udp_second != NULL) {
		if (select_is_change_sock_for_read(sock_server_udp_second->sock)) {
			client_eventUdpSelect(sock_server_udp_second);
			count++;
		}
	}

	return count;
}

void server_udp_quit()
{
	DEBUG_MSG(_("[Debug] Shutting down UDP\n"));

	if (sock_server_udp != NULL) {
		DEBUG_MSG(_("[Debug] Closing IPv4 [port %d]\n"), sock_udp_get_port(sock_server_udp));
		sock_udp_close(sock_server_udp);
	}

	if (sock_server_udp_second != NULL) {
		DEBUG_MSG(_("[Debug] Closing IPv6 [port %d]\n"), sock_udp_get_port(sock_server_udp));
		sock_udp_close(sock_server_udp_second);
	}
}
