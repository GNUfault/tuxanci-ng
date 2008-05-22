
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "main.h"
#include "arena.h"
#include "list.h"
#include "tux.h"
#include "shot.h"
#include "wall.h"
#include "item.h"
#include "teleport.h"
#include "pipe.h"
#include "myTimer.h"
#include "modules.h"

#ifndef PUBLIC_SERVER
#include "layer.h"
#endif

static arena_t *currentArena;

void setCurrentArena(arena_t *p)
{
	currentArena = p;
}

arena_t* getCurrentArena()
{
	return currentArena;
}

arena_t* newArena()
{
	arena_t *new;
	new = malloc( sizeof(arena_t) );
	
#ifndef PUBLIC_SERVER
	new->background = NULL;
	strcpy(new->music, "");
#endif

	new->listTimer = newList();
	new->listTux = newList();
	new->listShot = newList();
	new->listItem = newList();

	return new;
}

int conflictSpace(int x1,int y1,int w1,int h1,int x2,int y2,int w2,int h2)
{
	return (x1<x2+w2 && x2<x1+w1 && y1<y2+h2 && y2<y1+h1);
}

int isFreeSpace(arena_t *arena, int x, int y, int w, int h)
{
	if( isConflictWithListTux(arena->listTux, x, y, w, h) )return 0;
	if( isConflictWithListShot(arena->listShot, x, y, w, h) )return 0;
	if( isConflictWithListItem(arena->listItem, x, y, w, h) )return 0;
	if( isConflictModule(x, y, w, h) )return 0;
	//if( isConflictWithListWall(arena->listWall, x, y, w, h) )return 0;
	//if( isConflictWithListTeleport(arena->listTeleport, x, y, w, h) )return 0;
	//if( isConflictWithListPipe(arena->listPipe, x, y, w, h) )return 0;

	return 1;
}

void findFreeSpace(arena_t *arena, int *x, int *y, int w, int h)
{
	int z_x;
	int z_y;

	assert( x != NULL );
	assert( y != NULL );

	do{
		z_x = random() % WINDOW_SIZE_X;
		z_y = random() % (WINDOW_SIZE_Y-200);
	}while( isFreeSpace(arena, z_x, z_y ,w ,h) == 0 );

	*x = z_x;
	*y = z_y;
}

#ifndef PUBLIC_SERVER

void drawArena(arena_t *arena)
{
	tux_t *tux = NULL;

	addLayer(arena->background, 0, 0, 0, 0, WINDOW_SIZE_X, WINDOW_SIZE_Y, -100);

	drawListTux(arena->listTux);
	//drawListWall(arena->listWall);
	//drawListTeleport(arena->listTeleport);
	//drawListPipe(arena->listPipe);
	drawListShot(arena->listShot);
	drawListItem(arena->listItem);

/*
	tux = getControlTux( TUX_CONTROL_KEYBOARD_RIGHT );
*/

	if( tux == NULL )
	{
		drawLayerCenter(WINDOW_SIZE_X/2, WINDOW_SIZE_Y/2);
	}
	else
	{
		drawLayerCenter(tux->x, tux->y);
	}
}

#endif

void eventArena(arena_t *arena)
{
	int i;

	//eventConflictTuxWithTeleport(arena->listTux, arena->listTeleport);
	eventConflictTuxWithShot(arena->listTux, arena->listShot);

	for( i = 0 ; i < 4 ; i++)
	{
		eventMoveListShot(arena->listShot);
		//eventConflictShotWithWall(arena->listWall, arena->listShot);
		//eventConflictShotWithTeleport(arena->listTeleport, arena->listShot);
		//eventConflictShotWithPipe(arena->listPipe, arena->listShot);
		eventConflictShotWithItem(arena->listItem, arena->listShot);
	}

	eventListItem(arena->listItem);
	
	eventListTux(arena->listTux);

	eventTimer(arena->listTimer);
}

void destroyArena(arena_t *p)
{
	destroyListItem(p->listTux, destroyTux);
	destroyListItem(p->listShot, destroyShot);
	destroyListItem(p->listItem, destroyItem);
	destroyTimer(p->listTimer);
	free(p);
}
