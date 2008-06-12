
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <signal.h>
#include <time.h>

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
#include "publicServer.h"
#include "net_multiplayer.h"
#include "serverConfigFile.h"
#include "heightScore.h"
#include "modules.h"

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

int initPublicServer()
{
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

	if( initNetMuliplayer(NET_GAME_TYPE_SERVER,
		getSetting("IP", "--ip", "0.0.0.0") , atoi( getSetting("PORT", "--port", "2200") ) ) < 0 )
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
