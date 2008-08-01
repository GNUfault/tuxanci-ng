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
#else
#include <io.h>
#include <winsock2.h>
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
#include "screen_world.h"
#endif

#ifdef PUBLIC_SERVER
#include "publicServer.h"
#include "heightScore.h"
#endif

#include "tcp.h"
#include "tcp_server.h"

static sock_tcp_t *sock_server_tcp;
static sock_tcp_t *sock_server_tcp_second;

client_t* newTcpClient(sock_tcp_t *sock_tcp)
{
	client_t *new;
	char str_ip[STR_IP_SIZE];

	assert( sock_tcp != NULL );

	getSockTcpIp(sock_tcp, str_ip, STR_IP_SIZE);
	printf("new client TCP from %s %d\n", str_ip, getSockTcpPort(sock_tcp));
	
	new = newAnyClient();
	new->type = CLIENT_TYPE_TCP;
	new->socket_tcp = sock_tcp;
	new->buffer = newBuffer(4096);

	return new;
}

void destroyTcpClient(client_t *p)
{
	char str_ip[STR_IP_SIZE];

	eventMsgInCheckFront(p);

	getSockTcpIp(p->socket_tcp, str_ip, STR_IP_SIZE);
	printf("closeTCP connect %s %d\n", str_ip, getSockTcpPort(p->socket_tcp) );

	closeTcpSocket(p->socket_tcp);
	destroyBuffer(p->buffer);

	destroyAnyClient(p);
}

int initTcpServer(char *ip, int port, int proto)
{
	sock_server_tcp = bindTcpSocket(ip, port, proto);
	sock_server_tcp_second = NULL;

	if( sock_server_tcp == NULL )
	{
		return -1;
	}

	disableNagle(sock_server_tcp);
	printf("server listen TCP port %s %d\n", ip, port);

	return 0;
}

static void eventNewClient(sock_tcp_t *server_sock)
{
	list_t *listClient;
	sock_tcp_t *sock;
	client_t *client;

	listClient = getListServerClient();
	sock = getTcpNewClient(server_sock);
	disableNagle(sock);
	client = newTcpClient(sock);
	addList(listClient, client);
}

static void eventTcpClient(client_t *client)
{
	char buffer[STR_PROTO_SIZE];
	int ret;

	memset(buffer, 0, STR_PROTO_SIZE);

	ret = readTcpSocket(client->socket_tcp, buffer, STR_PROTO_SIZE-1);

	if( ret <= 0 )
	{
		client->status = NET_STATUS_ZOMBIE;
		return;
	}

	addBuffer(client->buffer, buffer, ret);

	while( getBufferLine(client->buffer, buffer, STR_PROTO_SIZE) >= 0 )
	{
		addList(client->listRecvMsg, strdup(buffer) );
	}
}

static int selectServerTcpSocket()
{
	list_t *listClient;
	struct timeval tv;
	fd_set readfds;
	fd_set errorfds;
	int max_fd;
	int ret;
	int count;
	int i;

	listClient = getListServerClient();

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

	if( sock_server_tcp != NULL )
	{
		FD_SET(sock_server_tcp->sock, &errorfds);
		FD_SET(sock_server_tcp->sock, &readfds);

		if( sock_server_tcp->sock > max_fd )
		{
			max_fd = sock_server_tcp->sock;
		}
	}

	for( i  = 0 ; i < listClient->count ; i++ )
	{
		client_t *client;

		client = (client_t *)listClient->list[i];

		if( client->status != NET_STATUS_OK )
		{
			continue;
		}

		FD_SET(client->socket_tcp->sock, &errorfds);
		FD_SET(client->socket_tcp->sock, &readfds);

		if( client->socket_tcp->sock > max_fd )
		{
			max_fd = client->socket_tcp->sock;
		}
	}


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
	
	if( sock_server_tcp != NULL )
	{
		if( FD_ISSET(sock_server_tcp->sock, &readfds) )
		{
			eventNewClient(sock_server_tcp);
			count = 1;
		}
	
		if( FD_ISSET(sock_server_tcp->sock, &errorfds) )
		{
			printf("error\n");
		}
	}

	for( i  = 0 ; i < listClient->count ; i++ )
	{
		client_t *client;

		client = (client_t *)listClient->list[i];

		if( FD_ISSET(client->socket_tcp->sock, &readfds) )
		{
			//printf("eventTcpClient(client);\n");
			eventTcpClient(client);
			count = 1;
		}

		if( FD_ISSET(client->socket_tcp->sock, &errorfds) )
		{
			//printf("error client\n");
			count = 1;
		}
	}

	return count;
}

void eventTcpServer()
{
#ifndef PUBLIC_SERVER

	int count;

#ifdef SUPPORT_NET_UNIX_UDP
	do{
		count = selectServerTcpSocket();
	}while( count > 0 );
#endif

#endif

#ifdef PUBLIC_SERVER
	selectServerTcpSocket();
#endif
}

void quitTcpServer()
{
	if( sock_server_tcp != NULL )
	{
		printf("close port %d\n", getSockTcpPort(sock_server_tcp));
		closeTcpSocket(sock_server_tcp);
	}

	if( sock_server_tcp_second != NULL )
	{
		printf("close port %d\n", getSockTcpPort(sock_server_tcp_second));
		closeTcpSocket(sock_server_tcp_second);
	}

	printf("quit TCP port\n");
}
