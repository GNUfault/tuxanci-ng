#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/time.h>

#include "main.h"
#include "list.h"
#include "tux.h"
#include "network.h"
#include "buffer.h"
#include "proto.h"
#include "server.h"
#include "assert.h"
#include "myTimer.h"
#include "arena.h"
#include "net_multiplayer.h"
#include "checkFront.h"

#ifndef PUBLIC_SERVER
#include "screen_world.h"
#endif

#ifdef PUBLIC_SERVER
#include "publicServer.h"
#include "heightScore.h"
#endif

#ifdef SUPPORT_NET_UNIX_UDP
#include "udp.h"
static sock_udp_t *sock_server_udp;
#endif

#ifdef SUPPORT_NET_SDL_UDP
#include "sdl_udp.h"
static sock_sdl_udp_t *sock_server_sdl_udp;
#endif

static list_t *listClient;
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

static void delZombieCLient(void *p_nothink)
{
	client_t *thisClient;
	my_time_t currentTime;
	int i;

 	currentTime = getMyTime();

	for( i = 0 ; i < listClient->count; i++)
	{
		thisClient = (client_t *) listClient->list[i];

		if( currentTime - thisClient->lastPing > SERVER_TIMEOUT )
		{
			proto_send_error_server(PROTO_SEND_ONE, thisClient, PROTO_ERROR_CODE_TIMEOUT);
			thisClient->status = NET_STATUS_ZOMBIE;
		}

		if( thisClient->status == NET_STATUS_ZOMBIE )
		{
			if( thisClient->tux != NULL )
			{
				proto_send_deltux_server(PROTO_SEND_ALL, NULL, thisClient);
			}

			delListItem(listClient, i, destroyClient);
		}
	}

}

static void eventPeriodicSyncClient(void *p_nothink)
{
	client_t *thisClientInfo;
	client_t *thisClientSend;
	tux_t *thisTux;
	int i, j;

	for( i = 0 ; i < listClient->count; i++)
	{
		thisClientSend = (client_t *) listClient->list[i];

		if( thisClientSend->tux == NULL )
		{
			continue;
		}

#ifndef PUBLIC_SERVER
		thisTux = getControlTux(TUX_CONTROL_KEYBOARD_RIGHT);
		if( isTuxSeesTux(thisClientSend->tux, thisTux)  )
		proto_send_newtux_server(PROTO_SEND_ONE, thisClientSend, thisTux);

#endif

		for( j = 0 ; j < listClient->count; j++)
		{
			thisClientInfo = (client_t *) listClient->list[j];
			thisTux = thisClientInfo->tux;

			if( thisTux != NULL &&
			    thisClientSend != thisClientInfo &&
			    isTuxSeesTux(thisClientSend->tux, thisTux) )
			{
				proto_send_newtux_server(PROTO_SEND_ONE,
					thisClientSend, thisTux);
			}
		}
	}
}

/*
static void eventPeriodicSyncSelfClient(void *p_nothink)
{
	client_t *thisClient;
	int i;

	for( i = 0 ; i < listClient->count; i++)
	{
		thisClient = (client_t *) listClient->list[i];

		if( thisClient->tux != NULL )
		{
			proto_send_newtux_server(PROTO_SEND_ONE, thisClient, thisClient->tux);
		}
	}
}
*/

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
	//addTaskToTimer(listServerTimer, TIMER_PERIODIC, eventPeriodicSyncSelfClient, NULL, 5000);
	addTaskToTimer(listServerTimer, TIMER_PERIODIC, eventSendPingClients, NULL, SERVER_TIME_PING);

}

void static initServer()
{
	startUpdateServer();
	listServerTimer = NULL;
	listClient = newList();
	setServerMaxClients(SERVER_MAX_CLIENTS);
	setServerTimer();
}

#ifdef SUPPORT_NET_UNIX_UDP
int initUdpServer(char *ip, int port)
{
	sock_server_udp = bindUdpSocket(ip, port);

	if( sock_server_udp == NULL )
	{
		return -1;
	}

	initServer();

	printf("server listen UDP port %s %d\n", ip, port);

	return 0;
}
#endif

#ifdef SUPPORT_NET_SDL_UDP
int initSdlUdpServer(int port)
{
	sock_server_sdl_udp = bindSdlUdpSocket(port);

	if( sock_server_sdl_udp == NULL )
	{
		return -1;
	}

	initServer();

	printf("server listen UDP port %d\n", port);

	return 0;
}
#endif

static client_t* newAnyClient()
{
	client_t *new;
	
	new = malloc( sizeof(client_t) );
	memset(new, 0, sizeof(client_t));
	
	new->status = NET_STATUS_OK;
	new->buffer = newBuffer(LIMIT_BUFFER);
	new->lastPing = getMyTime();
	new->listCheck = newCheckFront();

	return new;
}

#ifdef SUPPORT_NET_UNIX_UDP
client_t* newUdpClient(sock_udp_t *sock_udp)
{
	client_t *new;
	char str_ip[STR_IP_SIZE];

	assert( sock_udp != NULL );

	getSockUdpIp(sock_udp, str_ip);
	printf("new client from %s:%d\n", str_ip, getSockUdpPort(sock_udp));
	
	new = newAnyClient();
	new->socket_udp = sock_udp;

	return new;
}
#endif

#ifdef SUPPORT_NET_SDL_UDP
client_t* newSdlUdpClient(sock_sdl_udp_t *sock_sdl_udp)
{
	client_t *new;

	assert( sock_sdl_udp != NULL );

	printf("new client from %d\n", getSockSdlUdpPort(sock_sdl_udp));
	new = newAnyClient();
	new->socket_sdl_udp = sock_sdl_udp;

	return new;
}
#endif

void destroyClient(client_t *p)
{
//	int index;

	assert( p != NULL );

#ifdef SUPPORT_NET_UNIX_UDP
	closeUdpSocket(p->socket_udp);
#endif

#ifdef SUPPORT_NET_SDL_UDP
	closeSdlUdpSocket(p->socket_sdl_udp);
#endif

	destroyBuffer(p->buffer);
	destroyCheckFront(p->listCheck);


	if( p->tux != NULL )
	{
#ifdef PUBLIC_SERVER
		addPlayerInHighScore(p->tux->name, p->tux->score);
#endif
/*
		index = searchListItem(getCurrentArena()->listTux, p->tux);
		delListItem(getCurrentArena()->listTux, index, destroyTux);
*/
		delObjectFromSpaceWithMem(getCurrentArena()->spaceTux, p->tux, destroyTux);
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

#ifdef SUPPORT_NET_UNIX_UDP
		ret = writeUdpSocket(sock_server_udp, p->socket_udp, msg, strlen(msg));
#endif

#ifdef SUPPORT_NET_SDL_UDP
		ret = writeSdlUdpSocket(sock_server_sdl_udp, p->socket_sdl_udp, msg, strlen(msg));
#endif
	}
}

void sendAllClientBut(char *msg, client_t *p)
{
	client_t *thisClient;
	int i;

	assert( msg != NULL );

	for( i = 0 ; i < listClient->count; i++)
	{
		thisClient = (client_t *) listClient->list[i];

		if( thisClient->tux != NULL && thisClient != p )
		{
			sendClient(thisClient, msg);
		}
	}
}

void sendAllClient(char *msg)
{
	assert( msg != NULL );

	sendAllClientBut(msg, NULL);
}

void sendAllClientSeesTux(char *msg, tux_t *tux)
{
	int i;

	assert( msg != NULL );

	for( i = 0 ; i < listClient->count ; i++ )
	{
		client_t *thisClient;
		tux_t *thisTux;

		thisClient = (client_t *)listClient->list[i];
		thisTux = (tux_t *)thisClient->tux;

		if( tux != thisTux &&
		    isTuxSeesTux(thisTux, tux) )
		{
			sendClient(thisClient, msg);
		}
	}
}

void addMsgClient(client_t *p, char *msg, int id)
{
	assert( p != NULL );
	assert( msg != NULL );

	if( p->status != NET_STATUS_ZOMBIE )
	{
		addMsgInCheckFront(p->listCheck, msg, id);
	}
}

void addMsgAllClientBut(char *msg, client_t *p, int id)
{
	client_t *thisClient;
	int i;

	assert( msg != NULL );

	for( i = 0 ; i < listClient->count; i++)
	{
		thisClient = (client_t *) listClient->list[i];

		if( thisClient->tux != NULL && thisClient != p )
		{
			addMsgClient(thisClient, msg, id);
		}
	}
}

void addMsgAllClient(char *msg,  int id)
{
	assert( msg != NULL );

	addMsgAllClientBut(msg, NULL, id);
}

void addMsgAllClientSeesTux(char *msg, tux_t *tux, int id)
{
	int i;

	assert( msg != NULL );

	for( i = 0 ; i < listClient->count ; i++ )
	{
		client_t *thisClient;
		tux_t *thisTux;

		thisClient = (client_t *)listClient->list[i];
		thisTux = (tux_t *)thisClient->tux;

		if( tux != thisTux &&
		    isTuxSeesTux(thisTux, tux) )
		{
			addMsgClient(thisClient, msg, id);
		}
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

#ifdef SUPPORT_NET_UNIX_UDP

static void eventCreateNewUdpClient(sock_udp_t *socket_udp)
{
	client_t *client;

	assert( socket_udp != NULL );

	client = newUdpClient( socket_udp );
	addList(listClient, client);
}

#endif

#ifdef SUPPORT_NET_SDL_UDP

static void eventCreateNewSdlUdpClient(sock_sdl_udp_t *socket_sdl_udp)
{
	client_t *client;

	assert( socket_sdl_udp != NULL );

	client = newSdlUdpClient( socket_sdl_udp );
	addList(listClient, client);
}

#endif

static void eventClientBuffer(client_t *client)
{
	char line[STR_PROTO_SIZE];

	assert( client != NULL );

	/* obsluha udalosti od clientov */
	
	while( getBufferLine(client->buffer, line, STR_PROTO_SIZE) >= 0 )
	{
#ifndef PUBLIC_SERVER
		if( isParamFlag("--recv") )
		{
			printf("recv -> %s", line);
		}
#endif
		client->lastPing = getMyTime();

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

#ifdef SUPPORT_NET_UNIX_UDP

static client_t* findUdpClient(sock_udp_t *sock_udp)
{
	int i;
	client_t *thisClient;

	for( i = 0 ; i < listClient->count; i++)
	{
		thisClient = (client_t *) listClient->list[i];

		if( getSockUdpPort(thisClient->socket_udp) == getSockUdpPort(sock_udp) )
		{
			return thisClient;
		}
	}

	return NULL;
}

static void eventClientUdpSelect(sock_udp_t *sock_server)
{
	sock_udp_t *sock_client;
	client_t *client;
	char buffer[STR_SIZE];
	bool_t isCreateNewClient;
	int ret;

	assert( sock_server != NULL );

	sock_client = newSockUdp();
	isCreateNewClient = FALSE;

	memset(buffer, 0, STR_SIZE);

	ret = readUdpSocket(sock_server, sock_client, buffer, STR_SIZE-1);

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

	if( addBuffer(client->buffer, buffer, ret) != 0 )
	{
		client->status = NET_STATUS_ZOMBIE;
		return;
	}
}

void selectServerUdpSocket()
{
	struct timeval tv;
	fd_set readfds;
	int max_fd;
	int ret;

#ifndef PUBLIC_SERVER
	tv.tv_sec = 0;
	tv.tv_usec = 0;
#endif	

#ifdef PUBLIC_SERVER
	tv.tv_sec = 0;
	tv.tv_usec = 5000;
#endif	

	FD_ZERO(&readfds);
	FD_SET(sock_server_udp->sock, &readfds);
	max_fd = sock_server_udp->sock;

	if( listClient->count == 0 )
	{
		ret = select(max_fd+1, &readfds, (fd_set *)NULL, (fd_set *)NULL, NULL);
		setServerTimer();
	}
	else
	{
		ret = select(max_fd+1, &readfds, (fd_set *)NULL, (fd_set *)NULL, &tv);
	}

	if( ret < 0 )
	{
		return;
	}
	
	if( FD_ISSET(sock_server_udp->sock, &readfds) )
	{
		eventClientUdpSelect(sock_server_udp);
	}
}

#endif

#ifdef SUPPORT_NET_SDL_UDP

static client_t* findSdlUdpClient(sock_sdl_udp_t *sock_sdl_udp)
{
	int i;
	client_t *thisClient;

	for( i = 0 ; i < listClient->count; i++)
	{
		thisClient = (client_t *) listClient->list[i];

		if( getSockSdlUdpPort(thisClient->socket_sdl_udp) == getSockSdlUdpPort(sock_sdl_udp) )
		{
			return thisClient;
		}
	}

	return NULL;
}

void selectServerSdlUdpSocket()
{
	sock_sdl_udp_t *sock_client;
	client_t *client;
	char buffer[STR_PROTO_SIZE];
	bool_t isCreateNewClient;
	int ret;

	assert( sock_server_sdl_udp != NULL );

	do{
		sock_client = newSdlSockUdp();
		isCreateNewClient = FALSE;
	
		memset(buffer, 0, STR_PROTO_SIZE);
	
		ret = readSdlUdpSocket(sock_server_sdl_udp, sock_client, buffer, STR_PROTO_SIZE-1);
	
		if( ret < 0 )
		{
			destroySockSdlUdp(sock_client);
			break;
		}
	
		client = findSdlUdpClient(sock_client);
	
		if( client == NULL )
		{
			eventCreateNewSdlUdpClient(sock_client);
			client = findSdlUdpClient(sock_client);
			isCreateNewClient = TRUE;
		}
	
		if( client == NULL )
		{
			fprintf(stderr, "Client total not found !\n");
			return;
		}
		
		if( isCreateNewClient == FALSE )
		{
			destroySockSdlUdp(sock_client);
		}
	
		if( ret <= 0 )
		{
			client->status = NET_STATUS_ZOMBIE;
			return;
		}
	
		if( addBuffer(client->buffer, buffer, ret) != 0 )
		{
			client->status = NET_STATUS_ZOMBIE;
			return;
		}

	}while( ret > 0 );
}

#endif

void eventServer()
{
#ifdef SUPPORT_NET_SDL_UDP
	selectServerSdlUdpSocket();
#endif

#ifdef SUPPORT_NET_UNIX_UDP
	selectServerUdpSocket();
#endif

	eventClientListBuffer();
	eventTimer(listServerTimer);
}

static void quitServer()
{
	proto_send_end_server(PROTO_SEND_ALL, NULL);
	assert( listClient != NULL );
	destroyListItem(listClient, destroyClient);
	destroyTimer(listServerTimer);
}

#ifdef SUPPORT_NET_UNIX_UDP

void quitUdpServer()
{
	quitServer();

	assert( sock_server_udp != NULL );
	closeUdpSocket(sock_server_udp);

	printf("quit UDP port\n");
}

#endif

#ifdef SUPPORT_NET_SDL_UDP

void quitSdlUdpServer()
{
	quitServer();

	assert( sock_server_sdl_udp != NULL );
	closeSdlUdpSocket(sock_server_sdl_udp);

	printf("quit UDP port\n");
}

#endif
