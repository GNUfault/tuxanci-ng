
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <signal.h>
#include <time.h>

#include "main.h"
#include "list.h"
#include "tux.h"
#include "wall.h"
#include "teleport.h"
#include "pipe.h"
#include "myTimer.h"
#include "arena.h"
#include "item.h"
#include "shot.h"
#include "arenaFile.h"
#include "proto.h"
#include "publicServer.h"
#include "net_multiplayer.h"

static int arenaId;
static arena_t *arena;
static bool_t isSignalEnd;

static int my_argc;
static char **my_argv;

void countRoundInc()
{
}

int initPublicServer()
{
	initArenaFile();
	initTux();
	initItem();
	initShot();

	arenaId = getArenaIdFormNetName( getParamElse("--arena", "FAGN") );
	arena = getArena(arenaId);
	setCurrentArena(arena);

	addNewItem(arena->listItem, NULL);
	isSignalEnd = FALSE;

	if( initNetMuliplayer(NET_GAME_TYPE_SERVER,
		getParamElse("--ip", "127.0.0.1") , atoi( getParamElse("--port", "2200") ) ) < 0 )
	{
		printf("Nemozem inicalizovat sietovy socket !\n");
		return -1;
	}

	setServerMaxClients( atoi( getParamElse("--maxclients", "100") ));

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
	quitArenaFile();
	exit(0);
}

char* getParam(char *s)
{
	int i;
	int len;

	len = strlen(s);

	for( i = 1 ; i < my_argc ; i++ )
	{
		//printf("%s %s\n", s, my_argv[i]);
		
		if( strlen(my_argv[i]) < len )
		{
			continue;
		}

		if( strncmp(s, my_argv[i], len) == 0 )
		{
			return strchr(my_argv[i], '=')+1;
		}
	}

	return NULL;
}

char* getParamElse(char *s1, char *s2)
{
	char *ret;
	ret = getParam(s1);

	if( ret == NULL)
	{
		return s2;
	}

	return ret;
}

bool_t isParamFlag(char *s)
{
	int i;

	for( i = 0 ; i < my_argc ; i++ )
	{
		if( strcmp(s, my_argv[i]) == 0 )
		{
			return TRUE;
		}
	}

	return FALSE;
}

char *getString(int n)
{
	char str[STR_NUM_SIZE];
	sprintf(str, "%d", n);
	return strdup(str);
}

int main(int argc, char *argv[])
{
	srand( (unsigned) time(NULL) );

	my_argc = argc;
	my_argv = argv;

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


