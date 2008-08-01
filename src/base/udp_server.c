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

#include "udp.h"
#include "udp_server.h"

static sock_udp_t *sock_server_udp;
static sock_udp_t *sock_server_udp_second;

client_t* newUdpClient(sock_udp_t *sock_udp)
{
	client_t *new;
	char str_ip[STR_IP_SIZE];

	assert( sock_udp != NULL );

	getSockUdpIp(sock_udp, str_ip, STR_IP_SIZE);
	printf("new client UDP from %s %d\n", str_ip, getSockUdpPort(sock_udp));
	
	new = newAnyClient();
	new->type = CLIENT_TYPE_UDP;
	new->socket_udp = sock_udp;

	return new;
}

void destroyUdpClient(client_t *p)
{
	char str_ip[STR_IP_SIZE];

	eventMsgInCheckFront(p);

	getSockUdpIp(p->socket_udp, str_ip, STR_IP_SIZE);
	printf("close UDP connect %s %d\n", str_ip, getSockUdpPort(p->socket_udp) );

	destroySockUdp(p->socket_udp);

	destroyAnyClient(p);
}

int initUdpServer(char *ip, int port, int proto)
{
	sock_server_udp = bindUdpSocket(ip, port, proto);
	sock_server_udp_second = NULL;

	if( sock_server_udp == NULL )
	{
		return -1;
	}

	printf("server listen UDP port %s %d\n", ip, port);

	return 0;
}

#ifdef OLD_PUBLIC_SERVER

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

static void eventCreateNewUdpClient(sock_udp_t *socket_udp)
{
	list_t *listClient;
	client_t *client;

	assert( socket_udp != NULL );

	listClient = getListServerClient();

	client = newUdpClient(socket_udp);
	addList(listClient, client);
}

static client_t* findUdpClient(sock_udp_t *sock_udp)
{
	list_t *listClient;
	int port;
	int i;

	port  = getSockUdpPort(sock_udp);
	listClient = getListServerClient();

	for( i  = 0 ; i < listClient->count ; i++ )
	{
		client_t *client;

		client = (client_t *)listClient->list[i];

		if( getSockUdpPort(client->socket_udp) == port )
		{
			return client;
		}
	}

	return NULL;
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
		if( getCurrentArena()->spaceTux->list->count+1 > getServerMaxClients() )
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

static int selectServerUdpSocket()
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
	list_t *listClient;
	listClient = getListServerClient();

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

void eventUdpServer()
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
}

void quitUdpServer()
{
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
