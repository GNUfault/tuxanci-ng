
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "main.h"
#include "list.h"
#include "tux.h"
#include "network.h"
#include "buffer.h"
#include "net_multiplayer.h"
#include "screen_world.h"
#include "proto.h"
#include "client.h"

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

static buffer_t *clientBuffer;
static my_time_t lastPing;
static my_time_t lastPingServerAlive;

static void initClient()
{
	char name[STR_NAME_SIZE];

	clientBuffer = newBuffer( LIMIT_BUFFER );
	lastPing = getMyTime();
	lastPingServerAlive = getMyTime();

	getSettingNameRight(name);
	proto_send_hello_client(name);
}

#ifdef SUPPORT_NET_UNIX_TCP

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

#endif

#ifdef SUPPORT_NET_UNIX_UDP

int initUdpClient(char *ip, int port)
{
	sock_server_udp = connectUdpSocket(ip, port);

	if( sock_server_udp == NULL )
	{
		return -1;
	}

	printf("connect UDP %s %d\n", ip, port);

	protocolType = NET_PROTOCOL_TYPE_UDP;
	initClient();

	return 0;
}

#endif

#ifdef SUPPORT_NET_SDL_UDP

int initSdlUdpClient(char *ip, int port)
{
	sock_server_sdl_udp = connectSdlUdpSocket(ip, port);

	if( sock_server_sdl_udp == NULL )
	{
		return -1;
	}

	printf("connect UDP %s %d\n", ip, port);

	protocolType = NET_PROTOCOL_TYPE_UDP;
	initClient();

	return 0;
}

#endif

void sendServer(char *msg)
{
	int ret;

	assert( msg != NULL );

#ifdef SUPPORT_NET_UNIX_TCP
	ret = writeTcpSocket(sock_server_tcp, msg, strlen(msg));

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
#endif

#ifdef SUPPORT_NET_UNIX_UDP
	ret = writeUdpSocket(sock_server_udp, sock_server_udp, msg, strlen(msg));
#endif

#ifdef SUPPORT_NET_SDL_UDP
	ret = writeSdlUdpSocket(sock_server_sdl_udp, sock_server_sdl_udp, msg, strlen(msg));
#endif

#ifdef DEBUG_CLIENT_SEND
	printf("send server msg->%s", msg);
#endif
}

static int eventServerSelect()
{
	char buffer[STR_PROTO_SIZE];
	int ret;

	memset(buffer,0 ,STR_PROTO_SIZE);

#ifdef SUPPORT_NET_UNIX_TCP
	ret = readTcpSocket(sock_server_tcp, buffer, STR_PROTO_SIZE-1);

	if( ret == 0 )
	{
		fprintf(stderr, "server uzatovril sietovy socket\n");
		setWorldEnd();
		return ret;
	}
	
	if( ret < 0 )
	{
		fprintf(stderr, "chyba, spojenie prerusene \n");
		setWorldEnd();
		return ret;
	}
#endif
	
#ifdef SUPPORT_NET_UNIX_UDP
	ret = readUdpSocket(sock_server_udp, sock_server_udp, buffer, STR_PROTO_SIZE-1);
#endif

#ifdef SUPPORT_NET_SDL_UDP
	ret = readSdlUdpSocket(sock_server_sdl_udp, sock_server_sdl_udp, buffer, STR_PROTO_SIZE-1);

	if( ret < 0 )
	{
		return ret;
	}
#endif

	if( addBuffer(clientBuffer, buffer, ret) != 0 )
	{
		fprintf(stderr, "chyba, nemozem zapisovat do mojho buffera !\n");
		setWorldEnd();
		return ret;
	}

	return ret;
}

void eventServerBuffer()
{
	char line[STR_PROTO_SIZE];

	/* obsluha udalosti od servera */
	
	assert( clientBuffer != NULL );

	while ( getBufferLine(clientBuffer, line, STR_PROTO_SIZE) >= 0 )
	{
#ifdef DEBUG_CLIENT_RECV
			printf("recv server msg->%s", line);
#endif

		if( strncmp(line, "init", 4) == 0 )proto_recv_init_client(line);
		if( strncmp(line, "event", 5) == 0 )proto_recv_event_client(line);
		if( strncmp(line, "newtux", 6) == 0 )proto_recv_newtux_client(line);
		if( strncmp(line, "deltux", 6) == 0 )proto_recv_deltux_client(line);
		if( strncmp(line, "additem", 7) == 0 )proto_recv_additem_client(line);
		if( strncmp(line, "shot", 4) == 0 )proto_recv_shot_client(line);
		if( strncmp(line, "kill", 4) == 0 )proto_recv_kill_client(line);
		if( strncmp(line, "score", 5) == 0 )proto_recv_score_client(line);
		if( strncmp(line, "ping", 4) == 0 )proto_recv_ping_client(line);
		if( strncmp(line, "end", 3) == 0 )proto_recv_end_client(line);
	}
}

#ifdef SUPPORT_NET_UNIX_TCP

void selectClientTcpSocket()
{
	fd_set readfds;
	struct timeval tv;
	int max_fd;
	bool_t isNext;

	do{
		isNext = FALSE;

		tv.tv_sec = 0;
		tv.tv_usec = 0;
		
		FD_ZERO(&readfds);
		FD_SET(sock_server_tcp->sock, &readfds);
		max_fd = sock_server_tcp->sock;
	
		select(max_fd+1, &readfds, (fd_set *)NULL, (fd_set *)NULL, &tv);
	
		if( FD_ISSET(sock_server_tcp->sock, &readfds) )
		{
			eventServerSelect();
			isNext = TRUE;
		}

	}while( isNext == TRUE );
}

#endif

#if defined SUPPORT_NET_UNIX_UDP || defined SUPPORT_NET_SDL_UDP

void eventPingServer()
{
	my_time_t currentTime;

 	currentTime = getMyTime();

	if( currentTime - lastPing > CLIENT_TIMEOUT )
	{
		proto_send_ping_client();
		lastPing = getMyTime();
	}
}

void refreshPingServerAlive()
{
	lastPingServerAlive = getMyTime();
}

bool_t isServerAlive()
{
	my_time_t currentTime;

 	currentTime = getMyTime();

	if( currentTime - lastPingServerAlive > SERVER_TIMEOUT_ALIVE )
	{
		return FALSE;
	}

	return TRUE;
}

#endif

#ifdef SUPPORT_NET_UNIX_UDP

void selectClientUdpSocket()
{
	fd_set readfds;
	struct timeval tv;
	int max_fd;
	bool_t isNext;

	if( isServerAlive() == FALSE )
	{
		fprintf(stderr, "server neodpoveda !\n");
		setWorldEnd();
		return;
	}

	do{
		isNext = FALSE;

		tv.tv_sec = 0;
		tv.tv_usec = 0;
		
		FD_ZERO(&readfds);
		FD_SET(sock_server_udp->sock, &readfds);
		max_fd = sock_server_udp->sock;
	
		select(max_fd+1, &readfds, (fd_set *)NULL, (fd_set *)NULL, &tv);
	
		if( FD_ISSET(sock_server_udp->sock, &readfds) )
		{
			eventServerSelect();
			isNext = TRUE;
		}

	}while( isNext == TRUE );

}

#endif

#ifdef SUPPORT_NET_SDL_UDP

void selectClientSdlUdpSocket()
{
	int ret;

	if( isServerAlive() == FALSE )
	{
		fprintf(stderr, "server neodpoveda !\n");
		setWorldEnd();
		return;
	}

	do{
		ret = eventServerSelect();
	}while( ret > 0);
}

#endif

static void quitClient()
{
	proto_send_end_client();
	assert( clientBuffer != NULL );
	destroyBuffer(clientBuffer);
}

#ifdef SUPPORT_NET_UNIX_TCP

void quitTcpClient()
{
	quitClient();

	assert( sock_server_tcp != NULL );
	closeTcpSocket(sock_server_tcp);

	printf("quit TCP conenct\n");
}

#endif

#ifdef SUPPORT_NET_UNIX_UDP

void quitUdpClient()
{
	quitClient();

	assert( sock_server_udp != NULL );
	closeUdpSocket(sock_server_udp);

	printf("quit UDP conenct\n");
}

#endif

#ifdef SUPPORT_NET_SDL_UDP

void quitSdlUdpClient()
{
	quitClient();

	assert( sock_server_sdl_udp != NULL );
	closeSdlUdpSocket(sock_server_sdl_udp);

	printf("quit UDP conenct\n");
}

#endif
