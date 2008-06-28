
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <signal.h>
#include <time.h>

#include "base/main.h"
#include "base/list.h"
#include "base/tux.h"
#include "base/idManager.h"
#include "base/myTimer.h"
#include "base/arena.h"
#include "base/item.h"
#include "base/shot.h"
#include "base/arenaFile.h"
#include "base/proto.h"
#include "base/modules.h"
#include "base/net_multiplayer.h"

#include "server/publicServer.h"
#include "server/serverConfigFile.h"
#include "server/heightScore.h"

static int arenaId;
static arena_t *arena;
static bool_t isSignalEnd;

void countRoundInc()
{
}

static char *getSetting(char *env, char *param, char *default_val)
{
	return getParamElse(param, getServerConfigFileValue(env, default_val) );
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

	addNewItem(arena->spaceItem, ID_UNKNOWN);
	isSignalEnd = FALSE;

	ret = initPublicServerNetwork();

	if( ret < 0 )
	{
		printf("Nemozem inicalizovat sietovy socket !\n");
		return -1;
	}

	setServerMaxClients( atoi( getSetting("MAX_CLIENTS", "--maxclients", "32") ));

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

/*
	printf("interval = %d\n", interval);
*/
	lastActive = getMyTime();

	//fce_interval();
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
	signal(SIGPIPE, SIG_IGN);
	signal(SIGINT, my_handler_quit);
	signal(SIGTERM, my_handler_quit);
	signal(SIGQUIT, my_handler_quit);

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
