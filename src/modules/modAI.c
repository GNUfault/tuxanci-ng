
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "main.h"
#include "modules.h"
#include "tux.h"
#include "shot.h"
#include "list.h"
#include "gun.h"
#include "space.h"

#ifndef PUBLIC_SERVER
  #include "interface.h"
  #include "image.h"
#else
  #include "publicServer.h"
#endif

static export_fce_t *export_fce;

static void cmd_ai(char *line)
{
}

int init(export_fce_t *p)
{
	export_fce = p;

	return 0;
}

#ifndef PUBLIC_SERVER
int draw(int x, int y, int w, int h)
{
	return 0;
}
#endif

/////////////////

int colisionAll(tux_t *tux, int px, int py)
{
	arena_t *arena;
	int x, y, w, h;

	arena = export_fce->fce_getCurrentArena();

	export_fce->fce_getTuxProportion(tux, &x, &y, &w, &h);
	x += px;
	y += py;

	return export_fce->fce_isFreeSpace(arena, x, y, w, h);
}

void gotoTux(tux_t *tux, int x, int y, int w, int h)
{
	int px = 0,py = 0;
	int smer = TUX_DOWN;

	if(tux->x > x+w ) px = -TUX_STEP;
	if(tux->x < x ) px = +TUX_STEP;
	if(tux->y > y+h ) py = -TUX_STEP;
	if(tux->y < y ) py = +TUX_STEP;

	if( px != 0 && py !=0 ) px = 0;

	while( colisionAll(tux, px, 0 ) )
	{
		px=0;
		if(py==0) py = +TUX_STEP;
		goto sem;
	}

	while( colisionAll(tux, 0, py ) )
	{
		py = 0;
		if( px == 0 ) px = +TUX_STEP;
	}

	sem:
	if( px != 0 && py != 0 ) px = 0;

	if( py== -TUX_STEP )smer = TUX_UP;
	if( px== +TUX_STEP )smer = TUX_RIGHT;
	if( px== -TUX_STEP )smer = TUX_LEFT;
	if( py== +TUX_STEP )smer = TUX_DOWN;
	
	//printf("smer=%d\n",smer);
	export_fce->fce_actionTux(tux, smer);
}

void gotoTuxShot(tux_t *tux, int x, int y, int w, int h)
{
	if( tux->x > x && tux->x < x+w )
	{
		if( tux->y > y+h )tux->position = TUX_UP;
		if( tux->y < y )tux->position = TUX_DOWN;
	
		if( colisionAll(tux, 0, 0) == 0 )
		{
			if( tux->shot[GUN_SIMPLE] > 0 )tux->gun = GUN_SIMPLE;
			if( tux->shot[GUN_DUAL_SIMPLE] > 0 )tux->gun = GUN_DUAL_SIMPLE;
			if( tux->shot[GUN_SCATTER] > 0 )tux->gun = GUN_SCATTER;
			if( tux->shot[GUN_TOMMY] > 0 )tux->gun = GUN_TOMMY;
			if( tux->shot[GUN_LASSER] > 0 )tux->gun = GUN_LASSER;
			if( tux->shot[GUN_BOMBBALL] > 0 )tux->gun = GUN_BOMBBALL;

			export_fce->fce_actionTux(tux, TUX_SHOT);
		}
	}

	if( tux->y > y && tux->y < y+h )
	{
		if( tux->x > x+w )tux->position=TUX_LEFT;
		if( tux->x < x )tux->position=TUX_RIGHT;

		if( colisionAll(tux, 0 , 0) == 0 )
		{
			if( tux->shot[GUN_SIMPLE] > 0 )tux->gun = GUN_SIMPLE;
			if( tux->shot[GUN_DUAL_SIMPLE] > 0 )tux->gun = GUN_DUAL_SIMPLE;
			if( tux->shot[GUN_SCATTER] > 0 )tux->gun = GUN_SCATTER;
			if( tux->shot[GUN_TOMMY] > 0 )tux->gun = GUN_TOMMY;
			if( tux->shot[GUN_LASSER] > 0 )tux->gun = GUN_LASSER;
			if( tux->shot[GUN_BOMBBALL] > 0 )tux->gun = GUN_BOMBBALL;

			export_fce->fce_actionTux(tux, TUX_SHOT);
		}
	}
}

void killTux(tux_t *tux, int active)
{
	arena_t *arena;
	int i;

	arena = export_fce->fce_getCurrentArena();

	for( i = 0 ; i < arena->spaceTux->list->count ; i++ )
	{
		tux_t *thisTux;

		thisTux = arena->spaceTux->list->list[i];

		if( thisTux->control != TUX_CONTROL_KEYBOARD_LEFT &&
		    thisTux->status == TUX_STATUS_ALIVE )
		{
			gotoTuxShot(tux, thisTux->x, thisTux->y,
					TUX_WIDTH, TUX_HEIGHT);

			if( active )
			{
				gotoTux(tux, thisTux->x, thisTux->y,
				TUX_WIDTH, TUX_HEIGHT);

				gotoTuxShot(tux, thisTux->x, thisTux->y,
				TUX_WIDTH, TUX_HEIGHT);

				break;
			}
		}
	}
}

/////////////////

static void eventTux(tux_t *tux)
{
	arena_t *arena;
	item_t *item;

	arena = export_fce->fce_getCurrentArena();

	killTux(tux, 0);

	item = NULL;

	if( arena->spaceItem->list->count > 0  )
	{
		item = (item_t *) arena->spaceItem->list->list[0];
	}

	if( item != NULL && 
	    item->type != GUN_MINE )
	{
		gotoTux(tux, item->x, item->y,
		item->w, item->h);
	}
	else
	{
		killTux(tux, 1);
	}
}

int event()
{
	static my_time_t lastEvent = 0;
	my_time_t curentTime;
	arena_t *arena;
	int countTuxAI;
	int i;

	if( lastEvent == 0 )
	{
		lastEvent = export_fce->fce_getMyTime();
	}

	curentTime = export_fce->fce_getMyTime();

	if( curentTime - lastEvent < 25 )
	{
		return 0;
	}

	lastEvent = export_fce->fce_getMyTime();
	//printf("event AI\n");

	arena = export_fce->fce_getCurrentArena();
	countTuxAI = 0;

	for( i = 0 ; i < arena->spaceTux->list->count ; i++ )
	{
		tux_t *thisTux;

		thisTux = arena->spaceTux->list->list[i];

		if( thisTux->control == TUX_CONTROL_AI &&
		    thisTux->status == TUX_STATUS_ALIVE )
		{
			eventTux(thisTux);
			countTuxAI = 0;
		}
	}

	return 0;
}

int isConflict(int x, int y, int w, int h)
{
	return 0;
}

void cmdArena(char *line)
{
	if( strncmp(line, "ai", 2) == 0 )cmd_ai(line);
}

int destroy()
{
	return 0;
}
