
#ifndef __WIN32__
# include <sys/socket.h>
# include <sys/types.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <netdb.h>
#else
# include <windows.h>
# include <wininet.h>
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
#include "udp_server.h"
#include "tcp_server.h"
#include "myTimer.h"
#include "arena.h"
#include "net_multiplayer.h"
#include "checkFront.h"
#include "protect.h"
#include "index.h"

#ifndef PUBLIC_SERVER
#include "screen_world.h"
#endif

#ifdef PUBLIC_SERVER
#include "publicServer.h"
#include "heightScore.h"
#endif

#define SUPPORT_UDP

static list_t *listClient;

static list_t *listServerTimer;
static time_t timeUpdate;
static int maxClients;

typedef struct proto_cmd_server_struct
{
	char *name;
	int len;
	int tux;
	void (*fce_proto)(client_t *,char *msg);
} proto_cmd_server_t;

static proto_cmd_server_t proto_cmd_list[] =
{
	{ .name = "hello",	.len = 5,	.tux = 0,	.fce_proto = proto_recv_hello_server },
	{ .name = "status",	.len = 6,	.tux = 0,	.fce_proto = proto_recv_status_server },
#ifdef PUBLIC_SERVER
	{ .name = "list",	.len = 4,	.tux = 0,	.fce_proto = proto_recv_listscore_server },
#endif
	{ .name = "event",	.len = 5,	.tux = 1,	.fce_proto = proto_recv_event_server },
	{ .name = "check",	.len = 5,	.tux = 1,	.fce_proto = proto_recv_check_server },
	{ .name = "module",	.len = 6,	.tux = 1,	.fce_proto = proto_recv_module_server },
	{ .name = "chat",	.len = 4,	.tux = 1,	.fce_proto = proto_recv_chat_server },
	{ .name = "ping",	.len = 4,	.tux = 1,	.fce_proto = proto_recv_ping_server },
	{ .name = "end",	.len = 3,	.tux = 1,	.fce_proto = proto_recv_end_server },
	{ .name = "",		.len = 0,	.tux = 0,	.fce_proto = NULL },
};

static proto_cmd_server_t* findCmdProto(client_t *client, char *msg)
{
	int len;
	int i;

	len = strlen(msg);

	for( i = 0 ; proto_cmd_list[i].len != 0 ; i++ )
	{
		proto_cmd_server_t *thisCmd;
		
		thisCmd = &proto_cmd_list[i];

		if( len >= thisCmd->len && strncmp(msg, thisCmd->name, thisCmd->len) == 0 )
		{
			if( thisCmd->tux && client->tux == NULL )
			{
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

client_t* newAnyClient()
{
	client_t *new;

	new = malloc( sizeof(client_t) );
	memset(new, 0, sizeof(client_t));

	new->status = NET_STATUS_OK;
	new->listRecvMsg = newList();
	new->listSendMsg = newCheckFront();
	new->listSeesShot = newList();
	new->protect = newProtect();

	return new;
}

void destroyAnyClient(client_t *p)
{
	assert( p != NULL );

	//eventMsgInCheckFront(p);
	destroyListItem(p->listRecvMsg, free);
	destroyListItem(p->listSeesShot, free);
	destroyCheckFront(p->listSendMsg);

	destroyProtect(p->protect);

	if( p->tux != NULL )
	{
#ifdef PUBLIC_SERVER
		addPlayerInHighScore(p->tux->name, p->tux->score);
#endif
		delObjectFromSpaceWithObject(getCurrentArena()->spaceTux, p->tux, destroyTux);
	}

	free(p);
}

static void eventDelClient(client_t *client)
{
	int offset;

	offset =  searchListItem(listClient, client);

	assert( offset != -1 );

	switch( client->type )
	{
		case CLIENT_TYPE_UDP :
			delListItem(listClient, offset, destroyUdpClient);
		break;
		case CLIENT_TYPE_TCP :
			delListItem(listClient, offset, destroyTcpClient);
		break;
		default :
			assert( ! "zly typ !");
		break;
	}
}

static void delZombieCLient(void *p_nothink)
{
	client_t *thisClient;
	my_time_t currentTime;
	int i;

 	currentTime = getMyTime();

	for( i = 0 ; i < listClient->count; i++)
	{
		thisClient = (client_t *) listClient->list[i];

		if( isDown(thisClient->protect) == TRUE )
		{
			proto_send_end_server(PROTO_SEND_ONE, thisClient);
			eventMsgInCheckFront(thisClient);
			thisClient->status = NET_STATUS_ZOMBIE;
		}

		if( thisClient->status == NET_STATUS_ZOMBIE )
		{
			if( thisClient->tux != NULL )
			{
				proto_send_del_server(PROTO_SEND_ALL, NULL, thisClient->tux->id);
			}

			eventDelClient(thisClient);
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
		proto_send_newtux_server(PROTO_SEND_ALL_SEES_TUX, NULL, getControlTux(TUX_CONTROL_KEYBOARD_RIGHT));
#endif

	for( i = 0 ; i < listClient->count; i++ )
	{
		thisClientSend = (client_t *) listClient->list[i];
		thisTux = thisClientSend->tux;

		if( thisTux == NULL )
		{
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
	if( listServerTimer != NULL )
	{
		destroyTimer(listServerTimer);
	}

	restartTimer();
	listServerTimer = newTimer();

	addTaskToTimer(listServerTimer, TIMER_PERIODIC, delZombieCLient, NULL, SERVER_TIMEOUT);
	addTaskToTimer(listServerTimer, TIMER_PERIODIC, eventPeriodicSyncClient, NULL, SERVER_TIME_SYNC);
	addTaskToTimer(listServerTimer, TIMER_PERIODIC, eventSendPingClients, NULL, SERVER_TIME_PING);
}

int initServer(char *ip, int port, int proto)
{
	int ret;

	startUpdateServer();
	listServerTimer = NULL;

	listClient = newList();

	setServerMaxClients(SERVER_MAX_CLIENTS);
	setServerTimer();

#ifdef SUPPORT_UDP
	ret = initUdpServer(ip, port, proto);
#endif

#ifdef SUPPORT_TCP
	ret = initTcpServer(ip, port, proto);
#endif
	return ret;
}

list_t* getListServerClient()
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

void sendClient(client_t *p, char *msg)
{
	assert( p != NULL );
	assert( msg != NULL );

	if( p->status != NET_STATUS_ZOMBIE )
	{
		int ret;

#ifndef PUBLIC_SERVER
		if( isParamFlag("--send") )
		{
			printf("send -> %s", msg);
		}
#endif

		switch( p->type )
		{
			case CLIENT_TYPE_UDP :
				ret = writeUdpSocket(p->socket_udp, p->socket_udp, msg, strlen(msg));
			break;
			case CLIENT_TYPE_TCP :
				ret = writeTcpSocket(p->socket_tcp, msg, strlen(msg));
			break;
			default :
				assert( ! "zly typ !");
			break;
		}
	
		if( ret <= 0 )
		{
			p->status = NET_STATUS_ZOMBIE;
		}
	}
}

static void addMsgClient(client_t *p, char *msg, int type, int id)
{
	assert( p != NULL );
	assert( msg != NULL );

	if( p->status != NET_STATUS_ZOMBIE )
	{
		addMsgInCheckFront(p->listSendMsg, msg, type, id);
	}
}

static void addMsgAllClientBut(char *msg, client_t *p, int type, int id)
{
	client_t *thisClient;
	int i;

	assert( msg != NULL );

	for( i = 0 ; i < listClient->count; i++)
	{
		thisClient = (client_t *) listClient->list[i];

		if( thisClient->tux != NULL && thisClient != p )
		{
			addMsgClient(thisClient, msg, type, id);
		}
	}
}

static void addMsgAllClient(char *msg, int type, int id)
{
	assert( msg != NULL );

	addMsgAllClientBut(msg, NULL, type, id);
}

static void addMsgAllClientSeesTux(char *msg, tux_t *tux, int type, int id)
{
	list_t *listHelp;
	arena_t *arena;
	space_t *space;
	int x, y, w, h;
	int i;

	assert( msg != NULL );

	arena = getCurrentArena();
	space = arena->spaceTux;
	
	x = tux->x - WINDOW_SIZE_X;
	y = tux->y - WINDOW_SIZE_Y;

	w = 2 * WINDOW_SIZE_X;
	h = 2 * WINDOW_SIZE_Y;

	if( x < 0 )x = 0;
	if( y < 0 )y = 0;
	if( w+x >= arena->w )w = arena->w - (x+1);
	if( h+y >= arena->h )h = arena->h - (y+1);
	
	listHelp = newList();

	getObjectFromSpace(space, x, y, w, h, listHelp);
	//printf("%d %d %d %d %d\n", x, y, w, h, listHelp->count);

	for( i = 0 ; i < listHelp->count ; i++ )
	{
		tux_t *thisTux;
		client_t *thisClient;

		thisTux = (tux_t *)listHelp->list[i];

		if( thisTux == tux )
		{
			continue;
		}

		thisClient = thisTux->client;

		if( thisClient != NULL )
		{
			addMsgClient(thisClient, msg, type, id);
		}
	}

	destroyList(listHelp);
}

void protoSendClient(int type, client_t *client, char *msg, int type2, int id)
{
	assert( msg != NULL );

	switch( type )
	{
		case PROTO_SEND_ONE :
			assert( client != NULL );
			addMsgClient(client, msg, type2, id);
		break;
		case PROTO_SEND_ALL :
			assert( client == NULL );
			addMsgAllClient(msg, type2, id);
		break;
		case PROTO_SEND_BUT :
			assert( client != NULL );
			addMsgAllClientBut(msg, client, type2, id);
		break;
		case PROTO_SEND_ALL_SEES_TUX :
#ifndef PUBLIC_SERVER
			if( client != NULL )
			{
				addMsgAllClientSeesTux(msg, client->tux, type2, id);
			}
			else
			{
				addMsgAllClientSeesTux(msg, getControlTux(TUX_CONTROL_KEYBOARD_RIGHT), type2, id);
			}
#endif
#ifdef PUBLIC_SERVER
			addMsgAllClientSeesTux(msg, client->tux, type2, id);
#endif
		break;
		default :
			assert( ! "Premenna type ma zlu hodnotu !" );
		break;
	}
}

void sendInfoCreateClient(client_t *client)
{
	client_t *thisClient;
	tux_t *thisTux;
	item_t *thisItem;
	int i;

	assert( client != NULL );

	proto_send_init_server(PROTO_SEND_ONE, client, client);

#ifndef PUBLIC_SERVER
	proto_send_newtux_server(PROTO_SEND_ONE, client, getControlTux(TUX_CONTROL_KEYBOARD_RIGHT) );
#endif

	for( i = 0 ; i < listClient->count; i++)
	{
		thisClient = (client_t *) listClient->list[i];
		thisTux = thisClient->tux;

		if( thisTux != NULL && thisTux != client->tux )
		{
			proto_send_newtux_server(PROTO_SEND_ONE, thisClient, client->tux);
			proto_send_newtux_server(PROTO_SEND_ONE, client, thisTux);
		}
	}

	for( i = 0 ; i < getCurrentArena()->spaceItem->list->count; i++)
	{
		thisItem = (item_t *) getCurrentArena()->spaceItem->list->list[i];
		proto_send_additem_server(PROTO_SEND_ONE, client, thisItem);
	}
}

static void eventClientBuffer(client_t *client)
{
	proto_cmd_server_t* protoCmd;
	char *line;
	int i;

	assert( client != NULL );

	/* obsluha udalosti od clientov */
	
	for( i = 0 ; i < client->listRecvMsg->count ; i++ )
	{
		line = (char *)client->listRecvMsg->list[i];
		protoCmd = findCmdProto(client, line);

#ifndef PUBLIC_SERVER
		if( isParamFlag("--recv") )
		{
			printf("recv -> %s", line);
		}
#endif

		if( protoCmd != NULL )
		{
			protoCmd->fce_proto(client, line);
			rereshLastPing(client->protect);
		}
		else
		{
			proto_send_error_server(PROTO_SEND_ONE, client, PROTO_ERROR_CODE_BAD_COMMAND);
		}
	}

	destroyListItem(client->listRecvMsg, free);
	client->listRecvMsg = newList();
}

static void eventClientListBuffer()
{
	int i;
	client_t *thisClient;

	for( i = 0 ; i < listClient->count; i++)
	{
		thisClient = (client_t *) listClient->list[i];
		eventClientBuffer(thisClient);
		eventMsgInCheckFront(thisClient);
	}
}

void eventServer()
{
#ifdef SUPPORT_UDP
	eventUdpServer();
#endif

#ifdef SUPPORT_TCP
	eventTcpServer();
#endif

	eventClientListBuffer();
	eventTimer(listServerTimer);
}

void quitServer()
{
	proto_send_end_server(PROTO_SEND_ALL, NULL);

	assert( listClient != NULL );

#ifdef SUPPORT_UDP
	destroyListItem(listClient, destroyUdpClient);
#endif

#ifdef SUPPORT_TCP
	destroyListItem(listClient, destroyTcpClient);
#endif

	destroyTimer(listServerTimer);

#ifdef SUPPORT_UDP
	quitUdpServer();
#endif

#ifdef SUPPORT_TCP
	quitTcpServer();
#endif
}
