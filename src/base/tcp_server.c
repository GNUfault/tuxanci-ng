#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>

#ifndef __WIN32__
#    include <sys/ioctl.h>
#    include <sys/socket.h>
#    include <sys/select.h>
#else
#    include <io.h>
#    include <winsock2.h>
#endif

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
#    include "screen_world.h"
#endif

#ifdef PUBLIC_SERVER
#    include "publicServer.h"
#    include "highScore.h"
#    include "log.h"
#endif

#include "tcp.h"
#include "tcp_server.h"
#include "serverSelect.h"

static sock_tcp_t *sock_server_tcp;
static sock_tcp_t *sock_server_tcp_second;

client_t *serverTcp_new_client(sock_tcp_t * sock_tcp)
{
	client_t *new;

	assert(sock_tcp != NULL);

	new = server_new_any_client();
	new->type = CLIENT_TYPE_TCP;
	new->socket_tcp = sock_tcp;
	new->recvBuffer = buffer_new(SERVER_TCP_BUFFER_LIMIT);
	new->sendBuffer = buffer_new(SERVER_TCP_BUFFER_LIMIT);

#ifdef PUBLIC_SERVER
	char str_log[STR_LOG_SIZE];
	char str_ip[STR_IP_SIZE];

	sock_tcp_get_ip(sock_tcp, str_ip, STR_IP_SIZE);
	sprintf(str_log, _("New client \"%s\" on port \"%d\" connected"), str_ip,
			sock_tcp_get_port(sock_tcp));
	log_add(LOG_INF, str_log);
#endif

	return new;
}

void serverTcp_send_client_buffer(client_t * client)
{
	void *data;
	int len;
	int res;

	len = buffer_get_size(client->sendBuffer);

	if (len == 0) {
		return;
	}

	data = buffer_get_data(client->sendBuffer);

	res = sock_tcp_write(client->socket_tcp, data, len);
	//printf("sock_tcp_write = %d\n", res);

	if (res < 0) {
		client->status = NET_STATUS_ZOMBIE;
		return;
	}

	buffer_cut(client->sendBuffer, res);
}

void serverTcp_destroy_client(client_t * client)
{
	checkFront_event(client);
	serverTcp_send_client_buffer(client);

#ifdef PUBLIC_SERVER
	char str_log[STR_LOG_SIZE];
	char str_ip[STR_IP_SIZE];

	sock_tcp_get_ip(client->socket_tcp, str_ip, STR_IP_SIZE);
	sprintf(str_log, _("Client \"%s\" on port \"%d\" disconnected"), str_ip,
			sock_tcp_get_port(client->socket_tcp));
	log_add(LOG_INF, str_log);
#endif

	sock_tcp_close(client->socket_tcp);
	buffer_destroy(client->recvBuffer);
	buffer_destroy(client->sendBuffer);

	server_destroy_any_client(client);
}

int serverTcp_init(char *ip4, int port4, char *ip6, int port6)
{
	int ret;

	ret = 0;

	if (ip4 != NULL) {
		sock_server_tcp = sock_tcp_bind(ip4, port4, PROTO_UDPv4);

		if (sock_server_tcp != NULL) {
			ret++;
			sock_tcp_diable_nagle(sock_server_tcp);

			DEBUG_MSG(_("Starting server: \"%s\" on port: \"%d\"\n"), ip4, port4);
		} else {
			DEBUG_MSG(_("Starting server: \"%s\" on port: \"%d\" FAILED!\n"), ip4, port4);

		}
	}

	if (ip6 != NULL) {
		sock_server_tcp_second = sock_tcp_bind(ip6, port6, PROTO_UDPv6);

		if (sock_server_tcp_second != NULL) {
			ret++;
			sock_tcp_diable_nagle(sock_server_tcp_second);

			DEBUG_MSG(_("Starting server: \"%s\" on port: \"%d\"\n"), ip6, port6);
		} else {

			DEBUG_MSG(_("Starting server: \"%s\" on port: \"%d\" FAILED!\n"), ip6, port6);
		}
	}

	return ret;
}

static void eventNewClient(sock_tcp_t * server_sock)
{
	list_t *listClient;
	sock_tcp_t *sock;
	client_t *client;

	listClient = server_get_list_clients();

	sock = sock_tcp_accept(server_sock);
	sock_tcp_diable_nagle(sock);
	sock_tcp_set_non_block(sock);

	client = serverTcp_new_client(sock);
	list_add(listClient, client);
}

static void eventTcpClient(client_t * client)
{
	char buffer[STR_PROTO_SIZE];
	int ret;

	memset(buffer, 0, STR_PROTO_SIZE);

	ret = sock_tcp_read(client->socket_tcp, buffer, STR_PROTO_SIZE - 1);
	//printf("sock_tcp_read = %d\n", ret);

	if (ret <= 0) {
		client->status = NET_STATUS_ZOMBIE;
		return;
	}

	if (buffer_append(client->recvBuffer, buffer, ret) < 0) {
		client->status = NET_STATUS_ZOMBIE;
		return;
	}

	while (buffer_get_line(client->recvBuffer, buffer, STR_PROTO_SIZE) >= 0) {
		list_add(client->listRecvMsg, strdup(buffer));
	}
}

void serverTcp_set_select()
{
	list_t *listClient;
	int i;

	listClient = server_get_list_clients();

	if (sock_server_tcp != NULL) {
		select_add_sock_for_read(sock_server_tcp->sock);
	}

	if (sock_server_tcp_second != NULL) {
		select_add_sock_for_read(sock_server_tcp_second->sock);
	}

	for (i = 0; i < listClient->count; i++) {
		client_t *client;

		client = (client_t *) listClient->list[i];

		if (client->status != NET_STATUS_OK || client->type != CLIENT_TYPE_TCP) {
			continue;
		}

		select_add_sock_for_read(client->socket_tcp->sock);
	}
}

int serverTcp_select_sock()
{
	list_t *listClient;
	int count;
	int i;

	count = 0;

	if (sock_server_tcp != NULL) {
		if (select_is_change_sock_for_read(sock_server_tcp->sock)) {
			eventNewClient(sock_server_tcp);
			count++;
		}
	}

	if (sock_server_tcp_second != NULL) {
		if (select_is_change_sock_for_read(sock_server_tcp_second->sock)) {
			eventNewClient(sock_server_tcp_second);
			count++;
		}
	}

	listClient = server_get_list_clients();

	for (i = 0; i < listClient->count; i++) {
		client_t *client;

		client = (client_t *) listClient->list[i];

		if (client->status != NET_STATUS_OK || client->type != CLIENT_TYPE_TCP) {
			continue;
		}

		if (select_is_change_sock_for_read(client->socket_tcp->sock)) {
			//printf("sChangeSockInSelectRead\n");
			eventTcpClient(client);
			count++;
		}
/*
		if( select_is_change_sock_for_write(client->socket_tcp->sock) )
		{
			//printf("select_is_change_sock_for_write\n");
			serverTcp_send_client_buffer(client);
			count++;
		}
*/
	}

	return count;
}

void serverTcp_quit()
{
	if (sock_server_tcp != NULL) {
		DEBUG_MSG(_("Closing port: \"%d\"\n"), sock_tcp_get_port(sock_server_tcp));
		sock_tcp_close(sock_server_tcp);
	}

	if (sock_server_tcp_second != NULL) {
		DEBUG_MSG(_("Closing port: \"%d\"\n"), sock_tcp_get_port(sock_server_tcp_second));
		sock_tcp_close(sock_server_tcp_second);
	}

	DEBUG_MSG("Quitting TCP\n");
}
