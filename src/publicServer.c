
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "main.h"
#include "list.h"
#include "tux.h"
#include "wall.h"
#include "teleport.h"
#include "pipe.h"
#include "item.h"
#include "shot.h"
#include "myTimer.h"
#include "arena.h"
#include "arenaFile.h"
#include "proto.h"
#include "publicServer.h"
#include "net_multiplayer.h"

static int arenaId;
static arena_t *arena;

arena_t* getWorldArena()
{
	return arena;
}

void countRoundInc()
{
}

void initPublicServer()
{
	initArenaFile();
	initTux();
	initItem();
	initShot();
	initTimer();

	arenaId = getArenaIdFormNetName( getParamElse("--arena", "FAGN") );
	arena = getArena(arenaId);
	addNewItem(arena->listItem, NULL);

	initNetMuliplayer(NET_GAME_TYPE_SERVER, NULL, atoi( getParamElse("--port", "2200") ) );
}

int getChoiceArenaId()
{
	return arenaId;
}

int conflictSpace(int x1,int y1,int w1,int h1,int x2,int y2,int w2,int h2)
{
	return (x1<x2+w2 && x2<x1+w1 && y1<y2+h2 && y2<y1+h1);
}

int isFreeSpace(int x, int y, int w, int h)
{
	if ( isConflictWithListTux(arena->listTux, x, y, w, h) )return 0;
	if ( isConflictWithListWall(arena->listWall, x, y, w, h) )return 0;
	if ( isConflictWithListShot(arena->listShot, x, y, w, h) )return 0;
	if ( isConflictWithListItem(arena->listItem, x, y, w, h) )return 0;
	if ( isConflictWithListTeleport(arena->listTeleport, x, y, w, h) )return 0;
	if ( isConflictWithListPipe(arena->listPipe, x, y, w, h) )return 0;

	return 1;
}

void findFreeSpace(int *x, int *y, int w, int h)
{
	int z_x;
	int z_y;

	assert( x != NULL );
	assert( y != NULL );

	do{
		z_x = random() % WINDOW_SIZE_X;
		z_y = random() % (WINDOW_SIZE_Y-200);
	}while( isFreeSpace(z_x, z_y ,w ,h) == 0 );

	*x = z_x;
	*y = z_y;
}

void eventPublicServer()
{
	static my_time_t lastActive = 0;
	my_time_t interval;

	eventNetMultiplayer();

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

	eventConflictTuxWithTeleport(arena->listTux, arena->listTeleport);
	eventConflictTuxWithShot(arena->listTux, arena->listShot);

	eventConflictShotWithWall(arena->listWall, arena->listShot);
	eventConflictShotWithTeleport(arena->listTeleport, arena->listShot);
	eventConflictShotWithPipe(arena->listPipe, arena->listShot);
	eventConflictShotWithItem(arena->listItem, arena->listShot);

	eventMoveListShot(arena->listShot);
	eventListItem(arena->listItem);
	eventListTux(arena->listTux);

	eventTimer();
}

void quitPublicServer()
{
	printf("quit public server\n");
	quitNetMultiplayer();
	destroyArena(arena);
	quitTimer();
}
