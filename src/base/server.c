
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/time.h>

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
#include "screen_world.h"
#endif

#ifdef PUBLIC_SERVER
#include "publicServer.h"
#include "heightScore.h"
#endif

#include "udp.h"

static sock_udp_t *sock_server_udp;
static sock_udp_t *sock_server_udp_second;

static list_t *listClient;
static list_t *listClientIndex;

static list_t *listServerTimer;
static time_t timeUpdate;
static int maxClients;

static void startUpdateServer()
{
	timeUpdate = time(NULL);
}

time_t getUpdateServer()
{
	return time(NULL) - timeUpdate;
}

static void eventDelClient(client_t *client)
{
	int offset;

	offset =  getFormIndex(listClientIndex, getSockUdpPort(client->socket_udp));

	assert( offset != -1 );

	delFromIndex(listClientIndex, getSockUdpPort(client->socket_udp));
	delListItem(listClient, offset, destroyClient);
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

static void setServerTimer()
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


static void initServer()
{
	startUpdateServer();
	listServerTimer = NULL;

	listClient = newList();
	listClientIndex = newIndex();

	setServerMaxClients(SERVER_MAX_CLIENTS);
	setServerTimer();
}

int initUdpServer(char *ip, int port, int proto)
{
	sock_server_udp = bindUdpSocket(ip, port, proto);
	sock_server_udp_second = NULL;

	if( sock_server_udp == NULL )
	{
		return -1;
	}

	initServer();

	printf("server listen UDP port %s %d\n", ip, port);

	return 0;
}

#ifdef PUBLIC_SERVER

int initUdpPublicServer(char *ip4, int port4, char *ip6, int port6)
{
	sock_server_udp = NULL;
	sock_server_udp_second = NULL;

	if( ip4 != NULL )
	{
		sock_server_udp = bindUdpSocket(ip4, port4, PROTO_UDPv4);
	
		if( sock_server_udp == NULL )
		{
			return -1;
		}
	
		printf("server listen UDP port %s %d\n", ip4, port4);
	}

	if( ip6 != NULL )
	{
		sock_server_udp_second = bindUdpSocket(ip6, port6, PROTO_UDPv6);
	
		if( sock_server_udp_second == NULL )
		{
			return -1;
		}
	
		printf("server listen UDP port %s %d\n", ip6, port6);
	}

	initServer();

	return 0;
}

#endif

client_t* newUdpClient(sock_udp_t *sock_udp)
{
	client_t *new;
	char str_ip[STR_IP_SIZE];

	assert( sock_udp != NULL );

	getSockUdpIp(sock_udp, str_ip, STR_IP_SIZE);
	printf("new client from %s %d\n", str_ip, getSockUdpPort(sock_udp));
	
	new = malloc( sizeof(client_t) );
	memset(new, 0, sizeof(client_t));
	
	new->status = NET_STATUS_OK;
	new->listRecvMsg = newList();
	new->listSendMsg = newCheckFront();
	new->listSeesShot = newList();
	new->protect = newProtect();
	new->socket_udp = sock_udp;

	return new;
}

void destroyClient(client_t *p)
{
	char str_ip[STR_IP_SIZE];

	assert( p != NULL );

	eventMsgInCheckFront(p);

	getSockUdpIp(p->socket_udp, str_ip, STR_IP_SIZE);
	printf("close connect %s %d\n", str_ip, getSockUdpPort(p->socket_udp) );

	closeUdpSocket(p->socket_udp);

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

		if( p->socket_udp->proto == PROTO_UDPv4 && 
		    sock_server_udp != NULL &&
		    sock_server_udp->proto == PROTO_UDPv4 )
		{
			ret = writeUdpSocket(sock_server_udp, p->socket_udp, msg, strlen(msg));
		}

		if( p->socket_udp->proto == PROTO_UDPv6 &&
		    sock_server_udp != NULL &&
		    sock_server_udp->proto == PROTO_UDPv6 )
		{
			ret = writeUdpSocket(sock_server_udp, p->socket_udp, msg, strlen(msg));
		}

		if( p->socket_udp->proto == PROTO_UDPv6 &&
		    sock_server_udp_second != NULL &&
		    sock_server_udp_second->proto == PROTO_UDPv6 )
		{
			ret = writeUdpSocket(sock_server_udp_second, p->socket_udp, msg, strlen(msg));
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
	arena_t *arena;
	space_t *space;
	int x, y, w, h;
	int i;

	assert( msg != NULL );

	listDoEmpty(listHelp);

	arena = getCurrentArena();
	space = arena->spaceTux;
	
	x = tux->x - WINDOW_SIZE_X;
	y = tux->y - WINDOW_SIZE_Y;

	w = 2 * WINDOW_SIZE_X;
	h = 2 * WINDOW_SIZE_Y;

	if( x < 0 )
	{
		x = 0;
	}

	if( y < 0 )
	{
		y = 0;
	}

	if( w+x >= arena->w )
	{
		w = arena->w - (x+1);
	}

	if( h+y >= arena->h )
	{
		h = arena->h - (y+1);
	}

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

#if 0
	for( i = 0 ; i < listClient->count ; i++ )
	{
		client_t *thisClient;
		tux_t *thisTux;

		thisClient = (client_t *)listClient->list[i];
		thisTux = (tux_t *)thisClient->tux;

		if( thisTux == NULL )
		{
			continue;
		}

		if( tux != thisTux &&
		    isTuxSeesTux(thisTux, tux) )
		{
			addMsgClient(thisClient, msg, type, id);
		}
	}
#endif
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

tux_t *getServerTux()
{
	return NULL;
}

client_t *getClientFromTux(tux_t *tux)
{
	client_t *thisClient;
	int i;

	for( i = 0 ; i < listClient->count; i++)
	{
		thisClient = (client_t *) listClient->list[i];

		if( thisClient->tux == tux )
		{
			return thisClient;
		}
	}

	return NULL;
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

static void eventCreateNewUdpClient(sock_udp_t *socket_udp)
{
	client_t *client;
	int offset;

	assert( socket_udp != NULL );

	client = newUdpClient( socket_udp );

	offset = addToIndex(listClientIndex, getSockUdpPort(client->socket_udp) );

	if( offset == -1 )
	{
		return;
	}

	insList(listClient, offset, client);
}

static void eventClientBuffer(client_t *client)
{
	char *line;
	int i;

	assert( client != NULL );

	/* obsluha udalosti od clientov */
	
	for( i = 0 ; i < client->listRecvMsg->count ; i++ )
	{
		line = (char *)client->listRecvMsg->list[i];

#ifndef PUBLIC_SERVER
		if( isParamFlag("--recv") )
		{
			printf("recv -> %s", line);
		}
#endif

		rereshLastPing(client->protect);

		if( strncmp(line, "hello", 5) == 0 )
		{
			proto_recv_hello_server(client, line);
			continue;
		}

		if( strncmp(line, "status", 6) == 0 )
		{
			proto_recv_status_server(client, line);
			continue;
		}

#ifdef PUBLIC_SERVER
		if( strncmp(line, "list", 4) == 0 )
		{
			proto_recv_listscore_server(client, line);
			continue;
		}
#endif

		if( client->tux != NULL )
		{
			if( strncmp(line, "event", 5) == 0 )
			{
				proto_recv_event_server(client, line);
				continue;
			}

			if( strncmp(line, "check", 5) == 0 )
			{
				proto_recv_check_server(client, line);
				continue;
			}

			if( strncmp(line, "chat", 4) == 0 )
			{
				proto_recv_chat_server(client, line);
				continue;
			}

			if( strncmp(line, "ping", 4) == 0 )
			{
				proto_recv_ping_server(client, line);
				continue;
			}

			if( strncmp(line, "end", 3) == 0 )
			{
				proto_recv_end_server(client, line);
				continue;
			}
		}

		if( client->tux != NULL )
		{
			proto_send_error_server(PROTO_SEND_ONE, client, PROTO_ERROR_CODE_BAD_COMMAND);
		}
	}

	destroyListItem(client->listRecvMsg, free);
	client->listRecvMsg = newList();
}

void eventClientListBuffer()
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

static client_t* findUdpClient(sock_udp_t *sock_udp)
{
	int offset;

	offset =  getFormIndex(listClientIndex, getSockUdpPort(sock_udp));

	if( offset == -1 )
	{
		return NULL;
	}

	return (client_t *) listClient->list[offset];

}

static void eventClientUdpSelect(sock_udp_t *sock_server)
{
	sock_udp_t *sock_client;
	client_t *client;
	char listRecvMsg[STR_SIZE];
	bool_t isCreateNewClient;
	int ret;

	assert( sock_server != NULL );

	sock_client = newSockUdp(sock_server->proto);
	isCreateNewClient = FALSE;

	memset(listRecvMsg, 0, STR_SIZE);

	ret = readUdpSocket(sock_server, sock_client, listRecvMsg, STR_SIZE-1);

	client = findUdpClient(sock_client);

	if( client == NULL )
	{
		if( getCurrentArena()->spaceTux->list->count+1 > maxClients )
		{
			destroySockUdp(sock_client);
			return;
		}

		eventCreateNewUdpClient(sock_client);
		client = findUdpClient(sock_client);
		isCreateNewClient = TRUE;
	}

	if( client == NULL )
	{
		fprintf(stderr, "Client total not found !\n");
		return;
	}
	
	if( isCreateNewClient == FALSE )
	{
		destroySockUdp(sock_client);
	}

	if( ret <= 0 )
	{
		client->status = NET_STATUS_ZOMBIE;
		return;
	}

	//printf("add packet >>%s<<\n", listRecvMsg);
	addList(client->listRecvMsg, strdup(listRecvMsg) );
}

int selectServerUdpSocket()
{
	struct timeval tv;
	fd_set readfds;
	fd_set errorfds;
	int max_fd;
	int ret;
	int count;

#ifndef PUBLIC_SERVER
	tv.tv_sec = 0;
	tv.tv_usec = 0;
#endif	

#ifdef PUBLIC_SERVER
	tv.tv_sec = 0;
	tv.tv_usec = 1000;
#endif	

	FD_ZERO(&readfds);
	FD_ZERO(&errorfds);

	max_fd = 0;
	count = 0;

	if( sock_server_udp != NULL )
	{
		FD_SET(sock_server_udp->sock, &errorfds);
		FD_SET(sock_server_udp->sock, &readfds);

		if( sock_server_udp->sock > max_fd )
		{
			max_fd = sock_server_udp->sock;
		}
	}

	if( sock_server_udp_second != NULL )
	{
		FD_SET(sock_server_udp_second->sock, &readfds);
		FD_SET(sock_server_udp_second->sock, &errorfds);

		if( sock_server_udp_second->sock > max_fd )
		{
			max_fd = sock_server_udp_second->sock;
		}
	}

	//printf("select..\n");

#ifdef PUBLIC_SERVER
	if( listClient->count == 0 )
	{
		ret = select(max_fd+1, &readfds, (fd_set *)NULL, &errorfds,  NULL);
		setServerTimer();
	}
	else
	{
		ret = select(max_fd+1, &readfds, (fd_set *)NULL,&errorfds, &tv);
	}
#endif

#ifndef PUBLIC_SERVER
	ret = select(max_fd+1, &readfds, (fd_set *)NULL,&errorfds, &tv);
#endif

	if( ret < 0 )
	{
		//printf("select ret = %d\n", ret);
		return 0;
	}
	
	if( sock_server_udp != NULL )
	{
		if( FD_ISSET(sock_server_udp->sock, &readfds) )
		{
			eventClientUdpSelect(sock_server_udp);
			count = 1;
		}
	
		if( FD_ISSET(sock_server_udp->sock, &errorfds) )
		{
			printf("error\n");
		}
	}

	if( sock_server_udp_second != NULL )
	{
		if( FD_ISSET(sock_server_udp_second->sock, &readfds) )
		{
			eventClientUdpSelect(sock_server_udp_second);
			count = 1;
		}
	
		if( FD_ISSET(sock_server_udp_second->sock, &errorfds) )
		{
			printf("error\n");
		}
	}

	return count;
}

void eventServer()
{
#ifndef PUBLIC_SERVER

	int count;

#ifdef SUPPORT_NET_UNIX_UDP
	do{
		count = selectServerUdpSocket();
	}while( count > 0 );
#endif

#endif

#ifdef PUBLIC_SERVER
	selectServerUdpSocket();
#endif

	eventClientListBuffer();
	eventTimer(listServerTimer);
}

void quitUdpServer()
{
	proto_send_end_server(PROTO_SEND_ALL, NULL);

	assert( listClient != NULL );

	destroyListItem(listClient, destroyClient);
	destroyIndex(listClientIndex);

	destroyTimer(listServerTimer);

	if( sock_server_udp != NULL )
	{
		printf("close port %d\n", getSockUdpPort(sock_server_udp));
		closeUdpSocket(sock_server_udp);
	}

	if( sock_server_udp_second != NULL )
	{
		printf("close port %d\n", getSockUdpPort(sock_server_udp_second));
		closeUdpSocket(sock_server_udp_second);
	}

	printf("quit UDP port\n");
}
