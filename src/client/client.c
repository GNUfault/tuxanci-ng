
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <fcntl.h>
#include <sys/types.h>
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

#include "client.h"
#include "language.h"

#include "screen_analyze.h"
#include "screen_world.h"

#include "udp.h"
#include "tcp.h"
#include "buffer.h"

#define SUPPORT_UDP

static sock_udp_t *sock_server_udp;
static sock_tcp_t *sock_server_tcp;

static list_t *listRecvMsg;
static buffer_t *clientBuffer;
static my_time_t lastPing;
static my_time_t lastPingServerAlive;

typedef struct proto_cmd_client_struct
{
	char *name;
	int len;
	void (*fce_proto)(char *msg);
} proto_cmd_client_t;

static proto_cmd_client_t proto_cmd_list[] =
{
	{ .name = "error",	.len = 5,	.fce_proto = proto_recv_error_client },
	{ .name = "init",	.len = 4,	.fce_proto = proto_recv_init_client },
	{ .name = "event",	.len = 5,	.fce_proto = proto_recv_event_client },
	{ .name = "newtux",	.len = 6,	.fce_proto = proto_recv_newtux_client },
	{ .name = "del",	.len = 3,	.fce_proto = proto_recv_del_client },
	{ .name = "additem",	.len = 7,	.fce_proto = proto_recv_additem_client },
	{ .name = "shot",	.len = 4,	.fce_proto = proto_recv_shot_client },
	{ .name = "kill",	.len = 4,	.fce_proto = proto_recv_kill_client },
	{ .name = "module",	.len = 6,	.fce_proto = proto_recv_module_client },
	{ .name = "chat",	.len = 4,	.fce_proto = proto_recv_chat_client },
	{ .name = "ping",	.len = 4,	.fce_proto = proto_recv_ping_client },
	{ .name = "end",	.len = 3,	.fce_proto = proto_recv_end_client },
	{ .name = "",		.len = 0,	.fce_proto = NULL },
};

static proto_cmd_client_t* findCmdProto(char *msg)
{
	int len;
	int i;

	len = strlen(msg);

	for( i = 0 ; proto_cmd_list[i].len != 0 ; i++ )
	{
		proto_cmd_client_t *thisCmd;
		
		thisCmd = &proto_cmd_list[i];

		if( len >= thisCmd->len && strncmp(msg, thisCmd->name, thisCmd->len) == 0 )
		{
			return thisCmd;
		}
	}

	return NULL;
}

static int initUdpClient(char *ip, int port, int proto)
{
	sock_server_udp = connectUdpSocket(ip, port, proto);

	if( sock_server_udp == NULL )
	{
		return -1;
	}

	printf("connect UDP %s %d\n", ip, port);

	return 0;
}

static int initTcpClient(char *ip, int port, int proto)
{
	sock_server_tcp = connectTcpSocket(ip, port, proto);

	if( sock_server_tcp == NULL )
	{
		return -1;
	}

	disableNagle(sock_server_tcp);

	printf("connect TCP %s %d\n", ip, port);

	return 0;
}

int initClient(char *ip, int port, int proto)
{
	char name[STR_NAME_SIZE];
	int ret;

	listRecvMsg = newList();
	lastPing = getMyTime();
	lastPingServerAlive = getMyTime();
	clientBuffer = newBuffer(4096);

#ifdef SUPPORT_UDP
	ret = initUdpClient(ip, port, proto);
#endif

#ifdef SUPPORT_TCP
	ret = initTcpClient(ip, port, proto);
#endif

	if( ret < 0 )
	{
		return -1;
	}

	getSettingNameRight(name);
	proto_send_hello_client(name);

	return 0;
}

void sendServer(char *msg)
{
	int ret;

	assert( msg != NULL );

#ifndef PUBLIC_SERVER
	if( isParamFlag("--send") )
	{
		printf("send -> %s", msg);
	}
#endif

#ifdef SUPPORT_UDP
	ret = writeUdpSocket(sock_server_udp, sock_server_udp, msg, strlen(msg));
#endif

#ifdef SUPPORT_TCP
	ret = writeTcpSocket(sock_server_tcp, msg, strlen(msg));
#endif
}

static int eventServerSelect()
{
	char buffer[STR_PROTO_SIZE];
	int ret;

	memset(buffer, 0, STR_PROTO_SIZE);

#ifdef SUPPORT_UDP
	ret = readUdpSocket(sock_server_udp, sock_server_udp, buffer, STR_PROTO_SIZE-1);
#endif

#ifdef SUPPORT_TCP
	ret = readTcpSocket(sock_server_tcp, buffer, STR_PROTO_SIZE-1);
#endif

	if( ret <= 0 )
	{
		return ret;
	}

	addBuffer(clientBuffer, buffer, ret);

	while( getBufferLine(clientBuffer, buffer, STR_PROTO_SIZE) >= 0 )
	{
		if( strlen(buffer) > 0)
		{
			addList(listRecvMsg, strdup(buffer) );
		}
	}
	
	return ret;
}

static void eventServerBuffer()
{
	proto_cmd_client_t *protoCmd;
	char *line;
	int i;

	/* obsluha udalosti od servera */
	
	assert( listRecvMsg != NULL );

	for( i = 0 ; i < listRecvMsg->count ; i++ )
	{
		line = (char *)listRecvMsg->list[i];
		protoCmd = findCmdProto(line);

#ifndef PUBLIC_SERVER
		if( isParamFlag("--recv") )
		{
			printf("recv -> %s", line);
		}
#endif
		if( protoCmd != NULL )
		{
			protoCmd->fce_proto(line);
			lastPingServerAlive = getMyTime();
		}
	}

	destroyListItem(listRecvMsg, free);
	listRecvMsg = newList();
}

static void eventPingServer()
{
	my_time_t currentTime;

 	currentTime = getMyTime();

	if( currentTime - lastPing > CLIENT_TIMEOUT )
	{
		proto_send_ping_client();
		lastPing = getMyTime();
	}
}

static bool_t isServerAlive()
{
	my_time_t currentTime;

 	currentTime = getMyTime();

	if( currentTime - lastPingServerAlive > SERVER_TIMEOUT_ALIVE )
	{
		return FALSE;
	}

	return TRUE;
}

static void selectClientSocket()
{
	fd_set readfds;
	struct timeval tv;
	int max_fd;
	int sock;
	bool_t isNext;

	if( isServerAlive() == FALSE )
	{
		fprintf(stderr, "Server does not respond!\n");
		setMsgToAnalyze(getMyText("ERROR_SERVER_DONT_ALIVE"));
		setWorldEnd();
		return;
	}

	do{
		isNext = FALSE;

		tv.tv_sec = 0;
		tv.tv_usec = 0;
		
		FD_ZERO(&readfds);

#ifdef SUPPORT_UDP
		sock = sock_server_udp->sock;
#endif	

#ifdef SUPPORT_TCP
		sock = sock_server_tcp->sock;
#endif

		FD_SET(sock, &readfds);
		max_fd = sock;
		select(max_fd+1, &readfds, (fd_set *)NULL, (fd_set *)NULL, &tv);
	
		if( FD_ISSET(sock, &readfds) )
		{
			if( eventServerSelect() > 0 )
			{
				isNext = TRUE;
			}
		}

	}while( isNext == TRUE );
}

void eventClient()
{
	eventPingServer();
	selectClientSocket();
	eventServerBuffer();
}

static void quitUdpClient()
{
	assert( sock_server_udp != NULL );
	closeUdpSocket(sock_server_udp);

	printf("quit UDP conenct\n");
}

static void quitTcpClient()
{
	assert( sock_server_tcp != NULL );
	closeTcpSocket(sock_server_tcp);

	printf("quit TCP conenct\n");
}

void quitClient()
{
	proto_send_end_client();
	assert( listRecvMsg != NULL );
	destroyListItem(listRecvMsg, free);
	destroyBuffer(clientBuffer);

#ifdef SUPPORT_UDP
	quitUdpClient();
#endif

#ifdef SUPPORT_TCP
	quitTcpClient();
#endif
}

