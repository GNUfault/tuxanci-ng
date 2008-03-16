
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"
#include "tux.h"
#include "network.h"
#include "buffer.h"
#include "proto.h"
#include "server.h"
#include "assert.h"
#include "myTimer.h"
#include "net_multiplayer.h"

#ifndef BUBLIC_SERVER
#include "screen_world.h"
#endif

#ifdef BUBLIC_SERVER
#include "publicServer.h"
#endif

static int protocolType;

#ifdef SUPPORT_NET_UNIX_TCP
#include "tcp.h"
static sock_tcp_t *sock_server_tcp;
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
static my_time_t lastSyncClient;

void static initServer()
{
	listClient = newList();
 	lastSyncClient = getMyTime();
}

#ifdef SUPPORT_NET_UNIX_TCP
int initTcpServer(int port)
{
	protocolType = NET_PROTOCOL_TYPE_TCP;

	sock_server_tcp = bindTcpSocket(port);

	if( sock_server_tcp == NULL )
	{
		return -1;
	}

	initServer();

	printf("server listen TCP port %d\n", port);

	return 0;
}
#endif

#ifdef SUPPORT_NET_UNIX_UDP
int initUdpServer(int port)
{
	protocolType = NET_PROTOCOL_TYPE_UDP;

	sock_server_udp = bindUdpSocket(port);

	if( sock_server_udp == NULL )
	{
		return -1;
	}

	initServer();

	printf("server listen UDP port %d\n", port);

	return 0;
}
#endif

#ifdef SUPPORT_NET_SDL_UDP
int initSdlUdpServer(int port)
{
	protocolType = NET_PROTOCOL_TYPE_UDP;

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
	new->tux = newTux();
	new->tux->control = TUX_CONTROL_NET;
	addList(getWorldArena()->listTux, new->tux);

	return new;
}

#ifdef SUPPORT_NET_UNIX_TCP
client_t* newTcpClient(sock_tcp_t *sock_tcp)
{
	client_t *new;
	char str_ip[STR_IP_SIZE];
	
	assert( sock_tcp != NULL );

	getSockTcpIp(sock_tcp, str_ip);
	printf("new client from %s:%d\n", str_ip, getSockTcpPort(sock_tcp));

	new = newAnyClient();
	new->socket_tcp = sock_tcp;

	return new;
}
#endif

#ifdef SUPPORT_NET_UNIX_UDP
client_t* newUdpClient(sock_udp_t *sock_udp)
{
	client_t *new;

	assert( sock_udp != NULL );

	printf("new client from %d\n", getSockUdpPort(sock_udp));
	new = newAnyClient();
	new->lastPing = getMyTime();
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
	new->lastPing = getMyTime();
	new->socket_sdl_udp = sock_sdl_udp;

	return new;
}
#endif

void destroyClient(client_t *p)
{
	int index;

	assert( p != NULL );

#ifdef SUPPORT_NET_UNIX_TCP
	closeTcpSocket(p->socket_tcp);
#endif

#ifdef SUPPORT_NET_UNIX_UDP
	closeUdpSocket(p->socket_udp);
#endif

#ifdef SUPPORT_NET_SDL_UDP
	closeSdlUdpSocket(p->socket_sdl_udp);
#endif

	destroyBuffer(p->buffer);
	index = searchListItem(getWorldArena()->listTux, p->tux);
	delListItem(getWorldArena()->listTux, index, destroyTux);

	free(p);
}

void sendClient(client_t *p, char *msg)
{
	assert( p != NULL );
	assert( msg != NULL );

	if( p->status == NET_STATUS_OK )
	{
		int ret;

#ifdef SUPPORT_NET_UNIX_TCP
		ret = writeTcpSocket(p->socket_tcp, msg, strlen(msg));	
#endif

#ifdef SUPPORT_NET_UNIX_UDP
		ret = writeUdpSocket(sock_server_udp, p->socket_udp, msg, strlen(msg));
#endif

#ifdef SUPPORT_NET_SDL_UDP
		ret = writeSdlUdpSocket(sock_server_sdl_udp, p->socket_sdl_udp, msg, strlen(msg));
#endif

		if( ret == 0 )
		{
			fprintf(stderr, "client sa odpojil\n");
			p->status = NET_STATUS_ZOMBIE;
		}
	
		if( ret < 0 )
		{
			fprintf(stderr, "nastala chyba pri posielanie spravy clientovy\n");
			p->status = NET_STATUS_ZOMBIE;
		}
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

		if( thisClient != p )
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

static void eventCreateClient(client_t *client)
{
	client_t *thisClient;
	tux_t *thisTux;
	item_t *thisItem;
	int i;

	assert( client != NULL );

	proto_send_init_server(client);

#ifndef BUBLIC_SERVER
	proto_send_newtux_server(client, 
		(tux_t *)(getWorldArena()->listTux->list[SERVER_INDEX_ROOT_TUX]) );
#endif

	for( i = 0 ; i < listClient->count; i++)
	{
		thisClient = (client_t *) listClient->list[i];
		thisTux = thisClient->tux;

		if( thisTux != client->tux )
		{
			proto_send_newtux_server(thisClient, client->tux);
			proto_send_newtux_server(client, thisTux);
		}
	}

	for( i = 0 ; i < getWorldArena()->listItem->count; i++)
	{
		thisItem = (item_t *) getWorldArena()->listItem->list[i];
		proto_send_additem_server(thisItem);
	}
}

#ifdef SUPPORT_NET_UNIX_TCP

static void eventCreateNewTcpClient(sock_tcp_t *socket_tcp)
{
	client_t *client;

	assert( socket_tcp != NULL );

	client = newTcpClient( getTcpNewClient(socket_tcp) );
	addList(listClient, client);

	eventCreateClient(client);
}

#endif

#ifdef SUPPORT_NET_UNIX_UDP

static void eventCreateNewUdpClient(sock_udp_t *socket_udp)
{
	client_t *client;

	assert( socket_udp != NULL );

	client = newUdpClient( socket_udp );
	addList(listClient, client);

	eventCreateClient(client);
}

#endif

#ifdef SUPPORT_NET_SDL_UDP

static void eventCreateNewSdlUdpClient(sock_sdl_udp_t *socket_sdl_udp)
{
	client_t *client;

	assert( socket_sdl_udp != NULL );

	client = newSdlUdpClient( socket_sdl_udp );
	addList(listClient, client);

	eventCreateClient(client);
}

#endif

#ifdef SUPPORT_NET_UNIX_TCP

static void eventClientTcpSelect(client_t *client)
{
	char buffer[STR_SIZE];
	int ret;

	assert( client != NULL );

	memset(buffer, 0, STR_SIZE);
	ret = readTcpSocket(client->socket_tcp, buffer, STR_SIZE-1);

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

#endif

static void eventClientBuffer(client_t *client)
{
	char line[STR_SIZE];

	assert( client != NULL );

	/* obsluha udalosti od clientov */
	
	while( getBufferLine(client->buffer, line, STR_SIZE) >= 0 )
	{
 		//printf("spracuvavam %s", line);

		if( strncmp(line, "hello", 5) == 0 )proto_recv_hello_server(client, line);
		if( strncmp(line, "event", 5) == 0 )proto_recv_event_server(client, line);
		if( strncmp(line, "context", 7) == 0 )proto_recv_context_server(client, line);
		if( strncmp(line, "ping", 4) == 0 )proto_recv_ping_server(client, line);
		if( strncmp(line, "end", 3) == 0 )proto_recv_end_server(client, line);
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
	}
}

#ifdef SUPPORT_NET_UNIX_TCP

void selectServerTcpSocket()
{
	fd_set readfds;
	struct timeval tv;
	client_t *thisClient;
	int max_fd;
	int i;

	tv.tv_sec = 0;
	tv.tv_usec = 0;
	
	FD_ZERO(&readfds);
	FD_SET(sock_server_tcp->sock, &readfds);
	max_fd = sock_server_tcp->sock;

	for( i = 0 ; i < listClient->count; i++)
	{
		thisClient = (client_t *) listClient->list[i];

		FD_SET(thisClient->socket_tcp->sock, &readfds);
	
		if( thisClient->socket_tcp->sock > max_fd )
		{
			max_fd = thisClient->socket_tcp->sock;
		}
	}

	select(max_fd+1, &readfds, (fd_set *)NULL, (fd_set *)NULL, &tv);

	if( FD_ISSET(sock_server_tcp->sock, &readfds) )
	{
		eventCreateNewTcpClient(sock_server_tcp);
	}

	for( i = 0 ; i < listClient->count; i++)
	{
		thisClient = (client_t *) listClient->list[i];

		if( FD_ISSET(thisClient->socket_tcp->sock, &readfds) )
		{
			eventClientTcpSelect(thisClient);
		}

		if( thisClient->status == NET_STATUS_ZOMBIE )
		{
			proto_send_deltux_server(thisClient);
			delListItem(listClient, i, destroyClient);
		}
	}
}

#endif

#if defined SUPPORT_NET_UNIX_UDP || defined SUPPORT_NET_SDL_UDP

static void delZombieCLient()
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
			printf("client ping timeout\n");
			thisClient->status = NET_STATUS_ZOMBIE;
		}

		if( thisClient->status == NET_STATUS_ZOMBIE )
		{
			proto_send_deltux_server(thisClient);
			delListItem(listClient, i, destroyClient);
		}
	}
}

void eventPeriodicSyncClient()
{
	my_time_t currentTime;

 	currentTime = getMyTime();

	if( currentTime - lastSyncClient > SERVER_TIME_SYNC )
	{
		client_t *thisClientInfo;
		client_t *thisClientSend;
		tux_t *thisTux;
		int i, j;
	
#ifndef BUBLIC_SERVER
		proto_send_newtux_server(NULL,
			(tux_t *)(getWorldArena()->listTux->list[SERVER_INDEX_ROOT_TUX]));
#endif
		for( i = 0 ; i < listClient->count; i++)
		{
			thisClientInfo = (client_t *) listClient->list[i];

			for( j = 0 ; j < listClient->count; j++)
			{
				thisClientSend = (client_t *) listClient->list[j];
				thisTux = thisClientInfo->tux;

				if( thisClientSend != thisClientInfo &&
				    thisTux->status == TUX_STATUS_ALIVE )
				{
					proto_send_newtux_server(thisClientSend, thisTux);
				}
			}
		}

		lastSyncClient = getMyTime();
	}
}

#endif

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
	static struct timeval tv = { .tv_sec = 0, .tv_usec = 50000 };
	fd_set readfds;
	int max_fd;

#ifndef BUBLIC_SERVER
	tv.tv_sec = 0;
	tv.tv_usec = 0;
#endif	

#ifdef BUBLIC_SERVER
	tv.tv_sec = 0;
	tv.tv_usec = 50000;
#endif	
	
	FD_ZERO(&readfds);
	FD_SET(sock_server_udp->sock, &readfds);
	max_fd = sock_server_udp->sock;

	delZombieCLient();

	select(max_fd+1, &readfds, (fd_set *)NULL, (fd_set *)NULL, &tv);

/*
	printf("%d\n", tv.tv_usec);
*/
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
	char buffer[STR_SIZE];
	bool_t isCreateNewClient;
	int ret;

	assert( sock_server_sdl_udp != NULL );

	delZombieCLient();

	sock_client = newSdlSockUdp();
	isCreateNewClient = FALSE;

	memset(buffer, 0, STR_SIZE);
	ret = readSdlUdpSocket(sock_server_sdl_udp, sock_client, buffer, STR_SIZE-1);

	if( ret < 0 )
	{
		return;
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
}

#endif

static void quitServer()
{
	proto_send_end_server();
	assert( listClient != NULL );
	destroyListItem(listClient, destroyClient);
}

#ifdef SUPPORT_NET_UNIX_TCP

void quitTcpServer()
{
	quitServer();

	assert( sock_server_tcp != NULL );
	closeTcpSocket(sock_server_tcp);

	printf("quit TCP port\n");
}

#endif

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
