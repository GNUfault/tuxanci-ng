
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "base/main.h"
#include "base/arena.h"
#include "base/list.h"
#include "base/tux.h"
#include "base/shot.h"
#include "base/item.h"
#include "base/myTimer.h"
#include "base/modules.h"

#ifndef PUBLIC_SERVER
#include "screen/screen_world.h"
#include "client/layer.h"
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

arena_t* newArena(int w, int h)
{
	arena_t *new;
	int zone_w, zone_h;

	new = malloc( sizeof(arena_t) );
	
#ifndef PUBLIC_SERVER
	new->background = NULL;
	strcpy(new->music, "");
#endif

	new->w = w;
	new->h = h;

	new->listTimer = newList();

	if( w > 800 || h > 600)
	{
		zone_w = 320;
		zone_h = 240;
	}
	else
	{
		zone_w = 80;
		zone_h = 60;
	}

	new->spaceTux = newSpace(w, h, zone_w, zone_h, getStatusTux, setStatusTux);
	new->spaceItem = newSpace(w, h, zone_w, zone_h, getStatusItem, setStatusItem);
	new->spaceShot = newSpace(w, h, zone_w, zone_h, getStatusShot, setStatusShot);

	return new;
}

int conflictSpace(int x1,int y1,int w1,int h1,int x2,int y2,int w2,int h2)
{
	return (x1<x2+w2 && x2<x1+w1 && y1<y2+h2 && y2<y1+h1);
}

int isFreeSpace(arena_t *arena, int x, int y, int w, int h)
{
	if( isConflictWithObjectFromSpace(arena->spaceTux, x, y, w, h) )return 0;
	if( isConflictWithObjectFromSpace(arena->spaceShot, x, y, w, h) )return 0;
	if( isConflictWithObjectFromSpace(arena->spaceItem, x, y, w, h) )return 0;
	if( isConflictWithObjectFromSpace(arena->spaceShot, x, y, w, h) )return 0;

	if( isConflictModule(x, y, w, h) )return 0;

	return 1;
}

void findFreeSpace(arena_t *arena, int *x, int *y, int w, int h)
{
	int z_x;
	int z_y;

	assert( x != NULL );
	assert( y != NULL );

	do{
		z_x = random() % WINDOW_SIZE_X;//arena->w;
		z_y = random() % WINDOW_SIZE_Y;//arena->h;
	}while( isFreeSpace(arena, z_x, z_y ,w ,h) == 0 );

	*x = z_x;
	*y = z_y;
}

void getCenterScreen(int *screen_x, int *screen_y, int x, int y)
{
	arena_t *arena;

	arena = getCurrentArena();

	*screen_x = x - WINDOW_SIZE_X/2;
	*screen_y = y - WINDOW_SIZE_Y/2;

	if( *screen_x < 0 )
	{
		*screen_x = 0;
	}

	if( *screen_y < 0 )
	{
		*screen_y = 0;
	}

	if( *screen_x + WINDOW_SIZE_X >= arena->w )
	{
		*screen_x = arena->w - WINDOW_SIZE_X;
	}

	if( *screen_y + WINDOW_SIZE_Y >= arena->h )
	{
		*screen_y = arena->h - WINDOW_SIZE_Y;
	}
}

#ifndef PUBLIC_SERVER

void drawArena(arena_t *arena)
{
	int screen_x, screen_y;
	tux_t *tux = NULL;
	int i, j;

	tux = getControlTux(TUX_CONTROL_KEYBOARD_RIGHT);

	if( tux == NULL )
	{
		return;
	}
/*
	screen_x = tux->x - WINDOW_SIZE_X/2;
	screen_y = tux->y - WINDOW_SIZE_Y/2;
*/

	getCenterScreen(&screen_x, &screen_y, tux->x, tux->y);

	for( i = screen_y/arena->background->h ;
	     i <= screen_y/arena->background->h + WINDOW_SIZE_Y/arena->background->h ; i++ )
	{
		for( j = screen_x/arena->background->w ;
		     j <= screen_x/arena->background->w + WINDOW_SIZE_X/arena->background->w ; j++ )
		{
			addLayer(arena->background,
				j * arena->background->w,
				i * arena->background->h,
				0, 0, arena->background->w, arena->background->h, -100);
		}

	}

	listDoEmpty(listHelp);
	getObjectFromSpace(arena->spaceTux, screen_x, screen_y, WINDOW_SIZE_X, WINDOW_SIZE_Y, listHelp);
	drawListTux(listHelp);

	listDoEmpty(listHelp);
	getObjectFromSpace(arena->spaceItem, screen_x, screen_y, WINDOW_SIZE_X, WINDOW_SIZE_Y, listHelp);
	drawListItem(listHelp);

	listDoEmpty(listHelp);
	getObjectFromSpace(arena->spaceShot, screen_x, screen_y, WINDOW_SIZE_X, WINDOW_SIZE_Y, listHelp);
	drawListShot(listHelp);

	//printSpace(arena->spaceShot);

	drawModule(screen_x, screen_y, WINDOW_SIZE_X, WINDOW_SIZE_Y);
/*
	my_time_t t;

	t = getMyTime();
*/
	if( tux == NULL )
	{
		drawLayerCenter(WINDOW_SIZE_X/2, WINDOW_SIZE_Y/2);
	}
	else
	{
		drawLayerCenter(tux->x, tux->y);
	}

//	printf("time = %d\n", getMyTime() - t );
}

#endif

void eventArena(arena_t *arena)
{
	int i;

	//eventConflictTuxWithTeleport(arena->listTux, arena->listTeleport);
	//eventConflictTuxWithShot(arena);

	for( i = 0 ; i < 8 ; i++)
	{
		eventMoveListShot(arena);
		checkShotIsInTuxScreen(arena);
		//eventConflictShotWithWall(arena->listWall, arena->listShot);
		//eventConflictShotWithTeleport(arena->listTeleport, arena->listShot);
		//eventConflictShotWithPipe(arena->listPipe, arena->listShot);
		eventConflictShotWithItem(arena);
		eventConflictTuxWithShot(arena);
		eventModule();
	}

	eventListItem(arena->spaceItem);
	
	eventListTux(arena->spaceTux->list);

	eventTimer(arena->listTimer);
}

void destroyArena(arena_t *p)
{
	destroySpaceWithObject(p->spaceTux, destroyTux);
	destroySpaceWithObject(p->spaceItem, destroyItem);
	destroySpaceWithObject(p->spaceShot, destroyShot);
	destroyTimer(p->listTimer);
	free(p);
}
