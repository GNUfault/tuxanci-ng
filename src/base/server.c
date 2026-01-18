#ifndef __WIN32__
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#else /* __WIN32__ */
#include <windows.h>
#include <wininet.h>
#endif /* __WIN32__ */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <fcntl.h>

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
#include "serverSelect.h"
#include "serverSendMsg.h"
#include "downServer.h"

#ifndef PUBLIC_SERVER
#include "world.h"
#else /* PUBLIC_SERVER */
#include "publicServer.h"
#include "highScore.h"
#endif /* PUBLIC_SERVER */

#include "udp_server.h"

static list_t *listClient;

static list_t *listServerTimer;
static time_t timeUpdate;
static int maxClients;

typedef struct proto_cmd_server_struct {
	char *name;
	int len;
	int tux;
	void (*fce_proto) (client_t *, char *msg);
} proto_cmd_server_t;

static proto_cmd_server_t proto_cmd_list[] = {
	{.name = "hello",.len = 5,.tux = 0,.fce_proto = proto_recv_hello_server},
	{.name = "status",.len = 6,.tux = 0,.fce_proto = proto_recv_status_server},
#ifdef PUBLIC_SERVER
	{.name = "list",.len = 4,.tux = 0,.fce_proto = proto_recv_listscore_server},
#endif /* PUBLIC_SERVER */
	{.name = "event",.len = 5,.tux = 1,.fce_proto = proto_recv_event_server},
	{.name = "check",.len = 5,.tux = 1,.fce_proto = proto_recv_check_server},
	{.name = "module",.len = 6,.tux = 1,.fce_proto = proto_recv_module_server},
	{.name = "chat",.len = 4,.tux = 1,.fce_proto = proto_recv_chat_server},
	{.name = "ping",.len = 4,.tux = 1,.fce_proto = proto_recv_ping_server},
	{.name = "echo",.len = 4,.tux = 0,.fce_proto = proto_recv_echo_server},
	{.name = "end",.len = 3,.tux = 1,.fce_proto = proto_recv_end_server},
	{.name = "",.len = 0,.tux = 0,.fce_proto = NULL},
};

static proto_cmd_server_t *findCmdProto(client_t *client, char *msg)
{
	int len;
	int i;

	len = strlen(msg);

	for (i = 0; proto_cmd_list[i].len != 0; i++) {
		proto_cmd_server_t *thisCmd;

		thisCmd = &proto_cmd_list[i];

		if (len >= thisCmd->len && strncmp(msg, thisCmd->name, thisCmd->len) == 0) {
			if ((thisCmd->tux == 1 && client->tux == NULL) ||
			    (thisCmd->tux == 0 && client->tux != NULL)) {
				return NULL;
			}

			return thisCmd;
		}
	}

	return NULL;
}

static void startUpdateServer()
{
	timeUpdate = time(NULL);
}

time_t server_get_update()
{
	return time(NULL) - timeUpdate;
}

client_t *server_new_any_client()
{
	client_t *new;

	new = malloc(sizeof(client_t));
	memset(new, 0, sizeof(client_t));

	new->status = NET_STATUS_OK;
	new->listRecvMsg = list_new();
	new->listSendMsg = check_front_new();
	new->listSeesShot = list_new();
	new->protect = newProtect();

	return new;
}

void server_destroy_any_client(client_t *p)
{
	assert(p != NULL);

	/*check_front_event(p);*/
	list_destroy_item(p->listRecvMsg, free);
	list_destroy_item(p->listSeesShot, free);
	check_front_destroy(p->listSendMsg);

	destroyProtect(p->protect);

	if (p->tux != NULL) {
#ifdef PUBLIC_SERVER
		table_add(p->tux->name, p->tux->score);
#endif /* PUBLIC_SERVER */
		space_del_with_item(arena_get_current()->spaceTux, p->tux, tux_destroy);
	}

	free(p);
}

static void eventDelClientFromListClient(client_t *client)
{
	int offset;

	offset = list_search(listClient, client);

	assert(offset != -1);

	list_del_item(listClient, offset, server_udp_destroy_client);
}

static void delZombieCLient(void *p_nothing)
{
	client_t *thisClient;
	my_time_t currentTime;
        (void)currentTime; /* Fixed by GNUfault */
	int i;

	currentTime = timer_get_current_time();

	for (i = 0; i < listClient->count; i++) {
		thisClient = (client_t *) listClient->list[i];

		if (isDown(thisClient->protect) == TRUE) {
			proto_send_end_server(PROTO_SEND_ONE, thisClient);
			check_front_event(thisClient);
			thisClient->status = NET_STATUS_ZOMBIE;
		}

		if (thisClient->status == NET_STATUS_ZOMBIE) {
			if (thisClient->tux != NULL) {
				proto_send_del_server(PROTO_SEND_ALL, NULL, thisClient->tux->id);
			}

			eventDelClientFromListClient(thisClient);
			i--;
		}
	}
}

static void eventPeriodicSyncClient(void *p_nothing)
{
	client_t *thisClientSend;
	tux_t *thisTux;
	int i;

#ifndef PUBLIC_SERVER
	proto_send_newtux_server(PROTO_SEND_ALL_SEES_TUX, NULL, world_get_control_tux(TUX_CONTROL_KEYBOARD_RIGHT));
#endif /* PUBLIC_SERVER */

	for (i = 0; i < listClient->count; i++) {
		thisClientSend = (client_t *) listClient->list[i];
		thisTux = thisClientSend->tux;

		if (thisTux == NULL) {
			continue;
		}

		proto_send_newtux_server(PROTO_SEND_ALL_SEES_TUX, thisClientSend, thisTux);
		/*proto_send_newtux_server(PROTO_SEND_BUT, thisClientSend, thisTux);*/
	}
}

static void eventSendPingClients(void *p_nothing)
{
	proto_send_ping_server(PROTO_SEND_ALL, NULL);
}

void server_set_time()
{
	if (listServerTimer != NULL) {
		timer_destroy(listServerTimer);
	}

	timer_restart();
	listServerTimer = timer_new();

	timer_add_task(listServerTimer, TIMER_PERIODIC, delZombieCLient, NULL, SERVER_TIMEOUT);
	timer_add_task(listServerTimer, TIMER_PERIODIC, eventPeriodicSyncClient, NULL, SERVER_TIME_SYNC);
	timer_add_task(listServerTimer, TIMER_PERIODIC, eventSendPingClients, NULL, SERVER_TIME_PING);
}

int server_init(char *ip4, char *ip6, int port)
{
	int ret;

	startUpdateServer();
	listServerTimer = NULL;

	listClient = list_new();

	server_set_max_clients(SERVER_MAX_CLIENTS);
	server_set_time();

	ret = server_udp_init(ip4, ip6, port);

	if (ret == 0) {
		return -1;
	}

	/* initialize the arena download server */
	if (ip4 != NULL) {
		down_server_init(ip4, port);
	} else if (ip6 != NULL) {
		down_server_init(ip6, port);
	}

	return ret;
}

list_t *server_get_list_clients()
{
	return listClient;
}

int server_get_max_clients()
{
	return maxClients;
}

void server_set_max_clients(int n)
{
	maxClients = n;
}

void server_send_client(client_t *p, char *msg)
{
	assert(p != NULL);
	assert(msg != NULL);

	if (p->status != NET_STATUS_ZOMBIE) {
		int ret = -1;

#ifndef PUBLIC_SERVER
		if (isParamFlag("--send")) {
			debug("Sending: %s", msg);
		}
#endif /* PUBLIC_SERVER */

		ret = sock_udp_write(p->socket_udp, p->socket_udp, msg, strlen(msg));

		if (ret <= 0) {
			p->status = NET_STATUS_ZOMBIE;
		}
	}
}

static void client_eventWorkRecvList(client_t *client)
{
	proto_cmd_server_t *protoCmd;
	char *line;
	int i;

	assert(client != NULL);

	/* processing of events from clients */
	for (i = 0; i < client->listRecvMsg->count; i++) {
		line = (char *) client->listRecvMsg->list[i];
		protoCmd = findCmdProto(client, line);

#ifndef PUBLIC_SERVER
		if (isParamFlag("--recv")) {
			debug("Received: %s", line);
		}
#endif /* PUBLIC_SERVER */

		if (protoCmd != NULL) {
			protoCmd->fce_proto(client, line);
			rereshLastPing(client->protect);
		} else {
			proto_send_error_server(PROTO_SEND_ONE, client, PROTO_ERROR_CODE_BAD_COMMAND);
		}
	}

	list_destroy_item(client->listRecvMsg, free);
	client->listRecvMsg = list_new();
}

static void porcesListClients()
{
	int i;
	client_t *thisClient;

	for (i = 0; i < listClient->count; i++) {
		thisClient = (client_t *) listClient->list[i];

		client_eventWorkRecvList(thisClient);
		check_front_event(thisClient);
	}
}

void server_event()
{
#ifndef PUBLIC_SERVER
	int count;

	do {
		select_restart();

		server_udp_set_select();
		down_server_set_select();

		select_action();

		count = server_udp_select_sock();
		down_server_select_socket();
	} while (count > 0);
#else /* PUBLIC_SERVER */
	int ret;

	select_restart();

	server_udp_set_select();
	down_server_set_select();

	ret = select_action();

	if (ret > 0) {
		server_udp_select_sock();
		down_server_select_socket();
	}
#endif /* PUBLIC_SERVER */

	porcesListClients();
	timer_event(listServerTimer);
}

void server_quit()
{
	proto_send_end_server(PROTO_SEND_ALL, NULL);

	assert(listClient != NULL);

	list_destroy_item(listClient, server_udp_destroy_client);

	timer_destroy(listServerTimer);

	server_udp_quit();

	down_server_quit();
}
