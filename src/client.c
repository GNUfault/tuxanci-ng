
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "list.h"
#include "tux.h"
#include "network.h"
#include "buffer.h"
#include "net_multiplayer.h"
#include "screen_world.h"
#include "tcp.h"
#include "udp.h"
#include "proto.h"
#include "client.h"

static int protocolType;
static sock_tcp_t *sock_server_tcp;
static sock_udp_t *sock_server_udp;
static buffer_t *clientBuffer;
static Uint32 lastPing;

static void initClient()
{
	clientBuffer = newBuffer( LIMIT_BUFFER );
	proto_send_hello_client();
}

int initTcpClient(char *ip, int port)
{
	sock_server_tcp = connectTcpSocket(ip, port);

	if( sock_server_tcp == NULL )
	{
		return -1;
	}

	printf("connect TCP %s %d\n", ip, port);
	protocolType = NET_PROTOCOL_TYPE_TCP;
	initClient();

	return 0;
}

int initUdpClient(char *ip, int port)
{
	sock_server_udp = connectUdpSocket(ip, port);

	if( sock_server_udp == NULL )
	{
		return -1;
	}

	printf("connect UDP %s %d\n", ip, port);
	protocolType = NET_PROTOCOL_TYPE_UDP;
	lastPing = SDL_GetTicks();
	initClient();

	return 0;
}

void sendServer(char *msg)
{
	int ret;

	assert( msg != NULL );

	if( protocolType == NET_PROTOCOL_TYPE_TCP )
	{
		ret = writeTcpSocket(sock_server_tcp, msg, strlen(msg));
	}

	if( protocolType == NET_PROTOCOL_TYPE_UDP )
	{
		ret = writeUdpSocket(sock_server_udp, sock_server_udp, msg, strlen(msg));
	}

	if ( ret == 0 )
	{
		fprintf(stderr, "server uzatvoril sietovy socket\n");
		setWorldEnd();
	}
	
	if ( ret < 0 )
	{
		fprintf(stderr, "nastala chyba pri poslani spravy serveru\n");
		setWorldEnd();
	}
}

static void eventServerSelect()
{
	char buffer[STR_SIZE];
	int ret;

	memset(buffer,0 ,STR_SIZE);

	if( protocolType == NET_PROTOCOL_TYPE_TCP )
	{
		ret = readTcpSocket(sock_server_tcp, buffer, STR_SIZE-1);
	
	}

	if( protocolType == NET_PROTOCOL_TYPE_UDP )
	{
		ret = readUdpSocket(sock_server_udp, sock_server_udp, buffer, STR_SIZE-1);
	}

	if( ret == 0 )
	{
		fprintf(stderr, "server uzatovril sietovy socket\n");
		setWorldEnd();
		return;
	}
	
	if( ret < 0 )
	{
		fprintf(stderr, "chyba, spojenie prerusene \n");
		setWorldEnd();
		return;
	}

	if( addBuffer(clientBuffer, buffer, ret) != 0 )
	{
		fprintf(stderr, "chyba, nemozem zapisovat do mojho buffera !\n");
		setWorldEnd();
		return;
	}
}

void eventServerBuffer()
{
	char line[STR_SIZE];

	/* obsluha udalosti od servera */
	
	assert( clientBuffer != NULL );

	while ( getBufferLine(clientBuffer, line, STR_SIZE) >= 0 )
	{
		//printf("dostal som %s", line);

		if( strncmp(line, "init", 4) == 0 )proto_recv_init_client(line);
		if( strncmp(line, "event", 5) == 0 )proto_recv_event_client(line);
		if( strncmp(line, "newtux", 6) == 0 )proto_recv_newtux_client(line);
		if( strncmp(line, "deltux", 6) == 0 )proto_recv_deltux_client(line);
		if( strncmp(line, "additem", 7) == 0 )proto_recv_additem_client(line);
		if( strncmp(line, "kill", 4) == 0 )proto_recv_kill_client(line);
		if( strncmp(line, "score", 5) == 0 )proto_recv_score_client(line);
		if( strncmp(line, "end", 3) == 0 )proto_recv_end_client(line);
	}
}

void selectClientTcpSocket()
{
	fd_set readfds;
	struct timeval tv;
	int max_fd;

	tv.tv_sec = 0;
	tv.tv_usec = 0;
	
	FD_ZERO(&readfds);
	FD_SET(sock_server_tcp->sock, &readfds);
	max_fd = sock_server_tcp->sock;

	select(max_fd+1, &readfds, (fd_set *)NULL, (fd_set *)NULL, &tv);

	if( FD_ISSET(sock_server_tcp->sock, &readfds) )
	{
		eventServerSelect();
	}
}

void eventPingServer()
{
	Uint32 currentTime;

 	currentTime = SDL_GetTicks();

	if( currentTime - lastPing > CLIENT_TIMEOUT )
	{
		proto_send_ping_client();
		lastPing = SDL_GetTicks();
	}
}

void selectClientUdpSocket()
{
	fd_set readfds;
	struct timeval tv;
	int max_fd;

	eventPingServer();

	tv.tv_sec = 0;
	tv.tv_usec = 0;
	
	FD_ZERO(&readfds);
	FD_SET(sock_server_udp->sock, &readfds);
	max_fd = sock_server_udp->sock;

	select(max_fd+1, &readfds, (fd_set *)NULL, (fd_set *)NULL, &tv);

	if( FD_ISSET(sock_server_udp->sock, &readfds) )
	{
		eventServerSelect();
	}
}

static void quitClient()
{
	proto_send_end_client();
	assert( clientBuffer != NULL );
	destroyBuffer(clientBuffer);
}

void quitTcpClient()
{
	quitClient();

	assert( sock_server_tcp != NULL );
	closeTcpSocket(sock_server_tcp);

	printf("quit conenct\n");
}

void quitUdpClient()
{
	quitClient();

	assert( sock_server_udp != NULL );
	closeUdpSocket(sock_server_udp);

	printf("quit conenct\n");
}
