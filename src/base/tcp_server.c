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
#include "log.h"
#endif

#include "tcp.h"
#include "tcp_server.h"
#include "serverSelect.h"

static sock_tcp_t *sock_server_tcp;
static sock_tcp_t *sock_server_tcp_second;

client_t* newTcpClient(sock_tcp_t *sock_tcp)
{
	client_t *new;

	assert( sock_tcp != NULL );

	new = newAnyClient();
	new->type = CLIENT_TYPE_TCP;
	new->socket_tcp = sock_tcp;
	new->recvBuffer = newBuffer(SERVER_TCP_BUFFER_LIMIT);
	new->sendBuffer = newBuffer(SERVER_TCP_BUFFER_LIMIT);

#ifdef PUBLIC_SERVER
	char str_log[STR_LOG_SIZE];
	char str_ip[STR_IP_SIZE];

	getSockTcpIp(sock_tcp, str_ip, STR_IP_SIZE);
	sprintf(str_log, "new client TCP from %s %d", str_ip, getSockTcpPort(sock_tcp));
	addToLog(LOG_INF, str_log);
#endif

	return new;
}

void sendTcpClientBuffer(client_t *client)
{
	void *data;
	int len;
	int res;

	len = getBufferSize(client->sendBuffer);

	if( len == 0 )
	{
		return;
	}

	data = getBufferData(client->sendBuffer);

	res = writeTcpSocket(client->socket_tcp, data, len);

	if( res < 0 )
	{
		client->status = NET_STATUS_ZOMBIE;
		return;
	}

	cutBuffer(client->sendBuffer, res);
}

void destroyTcpClient(client_t *client)
{
	eventMsgInCheckFront(client);
	sendTcpClientBuffer(client);

#ifdef PUBLIC_SERVER
	char str_log[STR_LOG_SIZE];
	char str_ip[STR_IP_SIZE];

	getSockTcpIp(client->socket_tcp, str_ip, STR_IP_SIZE);
	sprintf(str_log, "close TCP connect %s %d", str_ip, getSockTcpPort(client->socket_tcp));
	addToLog(LOG_INF, str_log);
#endif

	closeTcpSocket(client->socket_tcp);
	destroyBuffer(client->recvBuffer);
	destroyBuffer(client->sendBuffer);

	destroyAnyClient(client);
}

int initTcpServer(char *ip4, int port4, char *ip6, int port6)
{
	int ret;

	ret = 0;

	if( ip4 != NULL )
	{
		sock_server_tcp = bindTcpSocket(ip4, port4, PROTO_UDPv4);

		if( sock_server_tcp != NULL )
		{
			ret++;
			disableNagle(sock_server_tcp);
			printf("server listen TCP port %s %d\n", ip4, port4);
		}
		else
		{
			printf("server listen TCP port %s %d -- failed !!!\n", ip4, port4);
		}
	}

	if( ip6 != NULL )
	{
		sock_server_tcp_second = bindTcpSocket(ip6, port6, PROTO_UDPv6);

		if( sock_server_tcp_second != NULL )
		{
			ret++;
			disableNagle(sock_server_tcp_second);
			printf("server listen TCP port %s %d\n", ip6, port6);
		}
		else
		{
			printf("server listen TCP port %s %d -- failed !!!\n", ip6, port6);
		}
	}

	return ret;
}

static void eventNewClient(sock_tcp_t *server_sock)
{
	list_t *listClient;
	sock_tcp_t *sock;
	client_t *client;

	listClient = getListServerClient();

	sock = getTcpNewClient(server_sock);
	disableNagle(sock);
	setTcpSockNonBlock(sock);

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

	addBuffer(client->recvBuffer, buffer, ret);

	while( getBufferLine(client->recvBuffer, buffer, STR_PROTO_SIZE) >= 0 )
	{
		addList(client->listRecvMsg, strdup(buffer) );
	}
}

void setServerTcpSelect()
{
	list_t *listClient;
	int i;

	listClient = getListServerClient();

	if( sock_server_tcp != NULL )
	{
		addSockToSelect(sock_server_tcp->sock);
	}

	if( sock_server_tcp_second != NULL )
	{
		addSockToSelect(sock_server_tcp_second->sock);
	}

	for( i  = 0 ; i < listClient->count ; i++ )
	{
		client_t *client;

		client = (client_t *)listClient->list[i];

		if( client->status != NET_STATUS_OK || client->type != CLIENT_TYPE_TCP )
		{
			continue;
		}

		addSockToSelect(client->socket_tcp->sock);
	}
}

int selectServerTcpSocket()
{
	list_t *listClient;
	int count;
	int i;

	count = 0;

	if( sock_server_tcp != NULL )
	{
		if( isChangeSockInSelect(sock_server_tcp->sock) )
		{
			eventNewClient(sock_server_tcp);
			count++;
		}
	}

	if( sock_server_tcp_second != NULL )
	{
		if( isChangeSockInSelect(sock_server_tcp_second->sock) )
		{
			eventNewClient(sock_server_tcp_second);
			count++;
		}
	}

	listClient = getListServerClient();

	for( i  = 0 ; i < listClient->count ; i++ )
	{
		client_t *client;

		client = (client_t *)listClient->list[i];

		if( client->status != NET_STATUS_OK || client->type != CLIENT_TYPE_TCP )
		{
			continue;
		}

		if( isChangeSockInSelect(client->socket_tcp->sock) )
		{
			//printf("eventTcpClient(client);\n");
			eventTcpClient(client);
			count++;
		}

		sendTcpClientBuffer(client);
	}

	return count;
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
