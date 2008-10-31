
#ifndef __WIN32__
#    include <sys/socket.h>
#    include <sys/types.h>
#    include <netinet/in.h>
#    include <arpa/inet.h>
#    include <netdb.h>
#else
#    include <windows.h>
#    include <wininet.h>
#endif

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
#    include "screen_world.h"
#endif

#ifdef PUBLIC_SERVER
#    include "publicServer.h"
#    include "highScore.h"
#endif


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
	{.name = "list",.len = 4,.tux = 0,.fce_proto =
	 proto_recv_listscore_server},
#endif
	{.name = "event",.len = 5,.tux = 1,.fce_proto = proto_recv_event_server},
	{.name = "check",.len = 5,.tux = 1,.fce_proto = proto_recv_check_server},
	{.name = "module",.len = 6,.tux = 1,.fce_proto = proto_recv_module_server},
	{.name = "chat",.len = 4,.tux = 1,.fce_proto = proto_recv_chat_server},
	{.name = "ping",.len = 4,.tux = 1,.fce_proto = proto_recv_ping_server},
	{.name = "echo",.len = 4,.tux = 0,.fce_proto = proto_recv_echo_server},
	{.name = "end",.len = 3,.tux = 1,.fce_proto = proto_recv_end_server},
	{.name = "",.len = 0,.tux = 0,.fce_proto = NULL},
};

static proto_cmd_server_t *findCmdProto(client_t * client, char *msg)
{
	int len;
	int i;

	len = strlen(msg);

	for (i = 0; proto_cmd_list[i].len != 0; i++) {
		proto_cmd_server_t *thisCmd;

		thisCmd = &proto_cmd_list[i];

		if (len >= thisCmd->len
			&& strncmp(msg, thisCmd->name, thisCmd->len) == 0) {
			if (thisCmd->tux && client->tux == NULL) {
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

time_t getUpdateServer()
{
	return time(NULL) - timeUpdate;
}

client_t *newAnyClient()
{
	client_t *new;

	new = malloc(sizeof(client_t));
	memset(new, 0, sizeof(client_t));

	new->status = NET_STATUS_OK;
	new->listRecvMsg = newList();
	new->listSendMsg = newCheckFront();
	new->listSeesShot = newList();
	new->protect = newProtect();

	return new;
}

void destroyAnyClient(client_t * p)
{
	assert(p != NULL);

	//eventMsgInCheckFront(p);
	destroyListItem(p->listRecvMsg, free);
	destroyListItem(p->listSeesShot, free);
	destroyCheckFront(p->listSendMsg);

	destroyProtect(p->protect);

	if (p->tux != NULL) {
#ifdef PUBLIC_SERVER
		addPlayerInHighScore(p->tux->name, p->tux->score);
#endif
		delObjectFromSpaceWithObject(getCurrentArena()->spaceTux, p->tux,
									 destroyTux);
	}

	free(p);
}

static void eventDelClientFromListClient(client_t * client)
{
	int offset;

	offset = searchListItem(listClient, client);

	assert(offset != -1);

	delListItem(listClient, offset, destroyUdpClient);
}

static void delZombieCLient(void *p_nothink)
{
	client_t *thisClient;
	my_time_t currentTime;
	int i;

	currentTime = getMyTime();

	for (i = 0; i < listClient->count; i++) {
		thisClient = (client_t *) listClient->list[i];

		if (isDown(thisClient->protect) == TRUE) {
			proto_send_end_server(PROTO_SEND_ONE, thisClient);
			eventMsgInCheckFront(thisClient);
			thisClient->status = NET_STATUS_ZOMBIE;
		}

		if (thisClient->status == NET_STATUS_ZOMBIE) {
			if (thisClient->tux != NULL) {
				proto_send_del_server(PROTO_SEND_ALL, NULL,
									  thisClient->tux->id);
			}

			eventDelClientFromListClient(thisClient);
			i--;
		}
	}
}

static void eventPeriodicSyncClient(void *p_nothink)
{
	client_t *thisClientSend;
	tux_t *thisTux;
	int i;

#ifndef PUBLIC_SERVER
	proto_send_newtux_server(PROTO_SEND_ALL_SEES_TUX, NULL,
							 getControlTux(TUX_CONTROL_KEYBOARD_RIGHT));
#endif

	for (i = 0; i < listClient->count; i++) {
		thisClientSend = (client_t *) listClient->list[i];
		thisTux = thisClientSend->tux;

		if (thisTux == NULL) {
			continue;
		}
		//proto_send_newtux_server(PROTO_SEND_ALL_SEES_TUX, thisClientSend, thisTux);
		proto_send_newtux_server(PROTO_SEND_BUT, thisClientSend, thisTux);
	}
}

static void eventSendPingClients(void *p_nothink)
{
	proto_send_ping_server(PROTO_SEND_ALL, NULL);
}

void setServerTimer()
{
	if (listServerTimer != NULL) {
		destroyTimer(listServerTimer);
	}

	restartTimer();
	listServerTimer = newTimer();

	addTaskToTimer(listServerTimer, TIMER_PERIODIC, delZombieCLient, NULL,
				   SERVER_TIMEOUT);
	addTaskToTimer(listServerTimer, TIMER_PERIODIC, eventPeriodicSyncClient,
				   NULL, SERVER_TIME_SYNC);
	addTaskToTimer(listServerTimer, TIMER_PERIODIC, eventSendPingClients,
				   NULL, SERVER_TIME_PING);
}

int initServer(char *ip4, int port4, char *ip6, int port6)
{
	int ret;

	startUpdateServer();
	listServerTimer = NULL;

	listClient = newList();

	setServerMaxClients(SERVER_MAX_CLIENTS);
	setServerTimer();

	ret = initUdpServer(ip4, port4, ip6, port6);

	if (ret == 0) {
		return -1;
	}

	initDownServer(ip4, port4);

	return ret;
}

list_t *getListServerClient()
{
	return listClient;
}

int getServerMaxClients()
{
	return maxClients;
}

void setServerMaxClients(int n)
{
	maxClients = n;
}

void sendClient(client_t * p, char *msg)
{
	assert(p != NULL);
	assert(msg != NULL);

	if (p->status != NET_STATUS_ZOMBIE) {
		int ret = -1;			// no Warnnings

#ifndef PUBLIC_SERVER
		if (isParamFlag("--send")) {
			printf(_("Sending: \"%s\""), msg);
		}
#endif

		ret = writeUdpSocket(p->socket_udp, p->socket_udp, msg, strlen(msg));

		if (ret <= 0) {
			p->status = NET_STATUS_ZOMBIE;
		}
	}
}

static void eventClientWorkRecvList(client_t * client)
{
	proto_cmd_server_t *protoCmd;
	char *line;
	int i;

	assert(client != NULL);

	/* obsluha udalosti od clientov */

	for (i = 0; i < client->listRecvMsg->count; i++) {
		line = (char *) client->listRecvMsg->list[i];
		protoCmd = findCmdProto(client, line);

#ifndef PUBLIC_SERVER
		if (isParamFlag("--recv")) {
			printf(_("Recieved: \"%s\""), line);
		}
#endif

		if (protoCmd != NULL) {
			protoCmd->fce_proto(client, line);
			rereshLastPing(client->protect);
		} else {
			proto_send_error_server(PROTO_SEND_ONE, client,
									PROTO_ERROR_CODE_BAD_COMMAND);
		}
	}

	destroyListItem(client->listRecvMsg, free);
	client->listRecvMsg = newList();
}

static void porcesListClients()
{
	int i;
	client_t *thisClient;

	for (i = 0; i < listClient->count; i++) {
		thisClient = (client_t *) listClient->list[i];

		eventClientWorkRecvList(thisClient);
		eventMsgInCheckFront(thisClient);
	}
}

void eventServer()
{
#ifndef PUBLIC_SERVER
	int count;

	do {
		restartSelect();

		setServerUdpSelect();
		setDownServerSelect();

		actionSelect();

		count = selectServerUdpSocket();
		selectDownServerSocket();
	} while (count > 0);
#endif

#ifdef PUBLIC_SERVER
	int ret;

	restartSelect();

	setServerUdpSelect();
	setDownServerSelect();

	ret = actionSelect();

	if (ret > 0) {
		selectServerUdpSocket();
		selectDownServerSocket();
	}
#endif

	porcesListClients();
	eventTimer(listServerTimer);
}

void quitServer()
{
	proto_send_end_server(PROTO_SEND_ALL, NULL);

	assert(listClient != NULL);

	destroyListItem(listClient, destroyUdpClient);

	destroyTimer(listServerTimer);

	quitUdpServer();

	quitDownServer();
}
