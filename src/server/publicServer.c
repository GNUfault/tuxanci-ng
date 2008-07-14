
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <signal.h>
#include <time.h>

#ifdef SUPPORT_NET_UNIX_UDP
#ifndef __WIN32
#include <sys/socket.h>
#include <sys/select.h>
#include <arpa/inet.h>
#else
# include <windows.h>
# include <wininet.h>
#endif

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#endif

#include "main.h"
#include "list.h"
#include "tux.h"
#include "idManager.h"
#include "myTimer.h"
#include "arena.h"
#include "item.h"
#include "shot.h"
#include "arenaFile.h"
#include "proto.h"
#include "modules.h"
#include "net_multiplayer.h"

#include "publicServer.h"
#include "serverConfigFile.h"
#include "heightScore.h"

#include "dns.h"

static int arenaId;
static arena_t *arena;
static bool_t isSignalEnd;

#ifdef SUPPORT_NET_UNIX_UDP
extern int errno;

#define	__PACKED__ __attribute__ ((__packed__))
#endif

void countRoundInc()
{
}

static char *getSetting(char *env, char *param, char *default_val)
{
	return getParamElse(param, getServerConfigFileValue(env, default_val) );
}

static int registerPublicServer()
{
#ifndef __WIN32
	int s;
#else
	SOCKET s;
#endif

	/* TODO: dodelat TCP makro */
#ifdef SUPPORT_NET_UNIX_UDP
	struct sockaddr_in server;
	char *master_server_ip;

	master_server_ip = getIPFormDNS(NET_MASTER_SERVER_DOMAIN);

	//printf("master_server_ip = %s\n", master_server_ip);

	if( master_server_ip == NULL ) // master server is down
	{
		return -1;
	}

	server.sin_family = AF_INET;
	server.sin_port = htons (NET_MASTER_PORT);
	server.sin_addr.s_addr = inet_addr (master_server_ip);
	
	free(master_server_ip);

	if ((s = socket (AF_INET, SOCK_STREAM, 0)) < 0) {
		return 0;
	}

	/* Set to nonblocking socket mode */
#ifndef __WIN32__
	int oldFlag;

	oldFlag = fcntl (s, F_GETFL, 0);

	if( fcntl(s, F_SETFL, oldFlag | O_NONBLOCK) == -1 )
	{
		return -1;
	}
#else
	unsigned long arg = 1;
	// Operation is  FIONBIO. Parameter is pointer on non-zero number.
	if( ioctlsocket(s, FIONBIO, &arg) == SOCKET_ERROR )
	{
		WSACleanup();
		return -1;
	}	
#endif

	if( connect (s, (struct sockaddr *) &server, sizeof (server)) == -1 ) {
#ifndef __WIN32__
		if (errno != EINPROGRESS)
		    return -1;
#else
		if (WSAGetLastError() != WSAEWOULDBLOCK) {
			WSACleanup();
			return -1;
		}
#endif
	}

	struct timeval tv;

	fd_set myset;

	FD_ZERO(&myset);
	FD_SET(s, &myset);

	tv.tv_sec = 3;
	tv.tv_usec = 0;

	int ret = select (s+1, NULL, &myset, NULL, &tv);

	if (ret == -1)
		return -1;

	if (ret == 0)
		return -1;

	FD_ZERO(&myset);
	FD_SET(s, &myset);

	tv.tv_sec = 1;
	tv.tv_usec = 0;

	ret = select (s+1, NULL, &myset, NULL, &tv);

	if (ret == -1)
		return -1;

	if (ret == 0)
		return -1;


	typedef struct {
		unsigned char cmd;
		unsigned port;
		unsigned ip;
	} __PACKED__ register_head;

	char *str = (char *) malloc (sizeof (register_head));

	register_head *head = (register_head *) str;

	head->cmd = 'p';
	head->port = atoi( getSetting("PORTv4", "--port", "2200") );
	head->ip = 0;	// TODO

	/* send request for server list */
	int r = send (s, str, 9, 0);

	free (str);

	if (r == -1) {
#ifndef __WIN32
		close(s);
#else
		closesocket(s);
#endif
		return -1;
	}

#ifndef __WIN32
		close(s);
#else
		closesocket(s);
#endif

#else
	return -1;
#endif

	return 0;
}

static int initPublicServerNetwork()
{
	char ip4[STR_IP_SIZE];
	char ip6[STR_IP_SIZE];
	char *p_ip4, *p_ip6;
	int port4;
	int port6;
	int ret;

	ret = -1;

	strcpy(ip4, getSetting("IP4", "--ip4", "none" ) );
	strcpy(ip6, getSetting("IP6", "--ip6", "none" ) );

	p_ip4 = ip4;

	if( strcmp(ip4, "none") == 0 )
	{
		p_ip4 = NULL;
	}

	p_ip6 = ip6;

	if( strcmp(ip6, "none") == 0 )
	{
		p_ip6= NULL;
	}

	port4 = atoi( getSetting("PORTv4", "--port", "2200") );
	port6 = atoi( getSetting("PORTv6", "--port", "2200") );

	ret = initNetMulitplayerPublicServer(p_ip4, port4, p_ip6, port6);

	return ret;
}

int initPublicServer()
{
	int ret;
	int i;

	initListID();
	initModule();
	initArenaFile();
	initTux();
	initItem();
	initShot();
	initServerConfigFile();
	initHeightScore( getSetting("SCOREFILE", "--scorefile", "./heightscore") );

	arenaId = getArenaIdFormNetName( getSetting("ARENA", "--arena", "FAGN") );
	arena = getArena(arenaId);
	setCurrentArena(arena);

	for( i = 0 ; i < atoi(getSetting("ITEM", "--item", "10")) ; i++ )
	{
		addNewItem(arena->spaceItem, ID_UNKNOWN);
	}

	isSignalEnd = FALSE;

	ret = initPublicServerNetwork();

	if( ret < 0 )
	{
		printf("Nemozem inicalizovat sietovy socket !\n");
		return -1;
	}

	setServerMaxClients( atoi( getSetting("MAX_CLIENTS", "--maxclients", "32") ));

	if (registerPublicServer() < 0)
	{
		printf("Nemuzu kontaktovat master server !\n");
	}

	return 0;
}

int getChoiceArenaId()
{
	return arenaId;
}

void eventPublicServer()
{
	static my_time_t lastActive = 0;
	my_time_t interval;

	eventNetMultiplayer();

	if( isSignalEnd == TRUE )
	{
		quitPublicServer();
	}

	if( lastActive == 0 )
	{
		lastActive = getMyTime();
	}

	interval = getMyTime() - lastActive;

	if( interval < 50 )
	{
		return;
	}

	//printf("interval = %d\n", interval);

	lastActive = getMyTime();

	eventArena(arena);
}

void my_handler_quit(int n)
{
	printf("my_handler_quit\n");
	isSignalEnd = TRUE;
}

void quitPublicServer()
{
	printf("quit public server\n");
	
	quitNetMultiplayer();
	destroyArena(arena);
	
	quitTux();
	quitItem();
	quitShot();

	quitArenaFile();
	quitServerConfigFile();
	quitModule();
	quitListID();
	quitHeightScore();
	destroyList(listHelp);

	exit(0);
}

int startPublicServer()
{
#ifndef __WIN32__
	signal(SIGPIPE, SIG_IGN);
	signal(SIGINT, my_handler_quit);
	signal(SIGTERM, my_handler_quit);
	signal(SIGQUIT, my_handler_quit);
#endif
	if( initPublicServer() < 0 )
	{
		quitPublicServer();
		return -1;
	}

	while(1)
	{
		eventPublicServer();
	}

	return 0;
}
