
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

typedef struct alternative_struct
{
	int first;
	int route;
	int step;
	int x, y;
} alternative_t;

alternative_t* newAlternative(int route, int x, int y)
{
	alternative_t *new;

	new = malloc( sizeof(alternative_t) );
	new->first = route;
	new->route = route;
	new->x = x;
	new->y = y;
	new->step = 0;

	return new;
}

alternative_t* cloneAlternative(alternative_t *p, int route, int x, int y)
{
	alternative_t *new;

	assert( p != NULL );
	
	new = newAlternative(route, p->x, p->y);
	new->first = p->first;
	new->step = p->step;

	return new;
}

void forkAlternative(list_t *list, alternative_t *p, int w, int h)
{
	int x, y;

	assert( list != NULL );
	assert( p != NULL );

	x  = p->x;
	y  = p->y;

	switch( p->route )
	{
		case TUX_UP :
			addList(list, cloneAlternative(p, TUX_RIGHT, x+(w+5), y ) );
			addList(list, cloneAlternative(p, TUX_LEFT, x-(w+5), y ) );
		break;
		case TUX_RIGHT :
			addList(list, cloneAlternative(p, TUX_UP, x, y-(h+5) ) );
			addList(list, cloneAlternative(p, TUX_DOWN, x, y+(h+5) ) );
		break;
		case TUX_LEFT :
			addList(list, cloneAlternative(p, TUX_UP, x, y-(h+5) ) );
			addList(list, cloneAlternative(p, TUX_DOWN, x, y+(h+5) ) );
		break;
		case TUX_DOWN :
			addList(list, cloneAlternative(p, TUX_RIGHT, x+(w+5), y ) );
			addList(list, cloneAlternative(p, TUX_LEFT, x-(w+5), y ) );
		break;
	}

}

void moveAlternative(alternative_t *p, int offset)
{
	assert( p != NULL );

	p->step++;

	//printf("move %d %d %d\n", p->x, p->y, p->step);

	switch( p->route )
	{
		case TUX_UP :
			p->y -= offset;
		break;
		case TUX_RIGHT :
			p->x += offset;
		break;
		case TUX_LEFT :
			p->x -= offset;
		break;
		case TUX_DOWN :
			p->y += offset;
		break;
	}
}

void destroyAlternative(alternative_t *p)
{
	assert( p != NULL );
	free(p);
}

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

tux_t *findOtherTux(space_t *space)
{
	int i;

	for( i = 0 ; i < getSpaceCount(space) ; i++ )
	{
		tux_t *thisTux;

		thisTux = getItemFromSpace(space, i);

		if( thisTux->control != TUX_CONTROL_AI )
		{
			return thisTux;
		}
	}

	return NULL;
}

static void shotTux(arena_t *arena, tux_t *tux_ai, tux_t *tux_rival)
{
	const int limit = 20;
	int x_ai, y_ai;
	int x_rival, y_rival;
	int w, h;

	export_fce->fce_getTuxProportion(tux_ai, &x_ai, &y_ai, &w, &h);
	export_fce->fce_getTuxProportion(tux_rival, &x_rival, &y_rival, &w, &h);

	if( y_rival < y_ai && x_rival > x_ai && x_rival < x_ai+limit )
	{
		export_fce->fce_actionTux(tux_ai, TUX_UP);
		export_fce->fce_actionTux(tux_ai, TUX_SHOT);
	}

	if( x_rival > x_ai && y_rival > y_ai && y_rival < y_ai+limit )
	{
		export_fce->fce_actionTux(tux_ai, TUX_RIGHT);
		export_fce->fce_actionTux(tux_ai, TUX_SHOT);
	}

	if( x_rival < x_ai && y_rival > y_ai && y_rival < y_ai+limit )
	{
		export_fce->fce_actionTux(tux_ai, TUX_LEFT);
		export_fce->fce_actionTux(tux_ai, TUX_SHOT);
	}

	if( y_rival > y_ai && x_rival > x_ai && x_rival < x_ai+limit )
	{
		export_fce->fce_actionTux(tux_ai, TUX_DOWN);
		export_fce->fce_actionTux(tux_ai, TUX_SHOT);
	}
}

static void eventTuxAI(tux_t *tux)
{
	arena_t *arena;
	tux_t *rivalTux;

	list_t *listAlternative;
	list_t *listDst;
	list_t *listFork;

	int x, y, w, h;
	int rival_x, rival_y;
	int i, index;

	int countFork = 0;
	int countDel = 0;
	int countLimit = 0;
	int countDo = 0;

	export_fce->fce_getTuxProportion(tux, &x, &y, &w, &h);
	//printf("tux AI %d %d\n", x, y);

	arena = export_fce->fce_getCurrentArena();

	rivalTux = findOtherTux(arena->spaceTux);

	if( rivalTux == NULL || rivalTux->status != TUX_STATUS_ALIVE )
	{
		return;
	}

	listAlternative = newList();
	listDst = newList();
	listFork = newList();

	shotTux(arena, tux, rivalTux);

	export_fce->fce_getTuxProportion(rivalTux, &rival_x, &rival_y, NULL, NULL);
	//printf("tux rival %d %d\n", rival_x, rival_y);

	addList(listAlternative, newAlternative(TUX_UP, x, y-(h+10)) );
	addList(listAlternative, newAlternative(TUX_RIGHT, x+(w+10), y) );
	addList(listAlternative, newAlternative(TUX_LEFT, x-(w+10), y) );
	addList(listAlternative, newAlternative(TUX_DOWN, x, y+(h+10)) );

	index = -1;
	while(1)
	{
		alternative_t *this;

		index++;
		if( index < 0 || index >= listAlternative->count )
		{

			int j;

			//printf("listFork->count = %d\n", listFork->count);

			for( j = 0; j < listFork->count ; j++ )
			{
				addList(listAlternative, listFork->list[j]);
			}

			listDoEmpty(listFork);

			index = 0;
		}

		if( listAlternative->count == 0 )
		{
			break;
		}

		this = (alternative_t *)listAlternative->list[index];
		
		if( ++countDo == 100 )break;

		if( this->step > 100 )
		{
			delListItem(listAlternative, index, destroyAlternative);
			countLimit++;
			index--;
			continue;
		}

		moveAlternative(this, w*2);

		if( export_fce->fce_isFreeSpace(arena, this->x, this->y, w, h) == 1 )
		{
			forkAlternative(listFork, this, 2*w, 2*h);
			countFork++;
			continue;
		}

		if( export_fce->fce_conflictSpace(this->x, this->y, w, h, rival_x, rival_y, w, h) )
		{
			//printf("this->step = %d\n", this->step);

 			delList(listAlternative, index);
			addList(listDst, this);
			index--;
			//continue;
			break;
		}

		if( export_fce->fce_isFreeSpace(arena, this->x, this->y, w, h) == 0 )
		{
			//forkAlternative(listFork, this, w*2, h*2);
			delListItem(listAlternative, index, destroyAlternative);
			index--;
			countDel++;
			
			continue;
		}

	}

	int minStep = 1000;
	int recRoute = 0;

	//printf("------------\n");
	for( i = 0 ; i < listDst->count ; i++ )
	{
		alternative_t *this;

		this = (alternative_t *)listDst->list[i];

		//printf("XXX step %d route = %d\n", this->step, this->first);
		if( this->step < minStep )
		{
			minStep = this->step;
			recRoute = this->first;
		}
	}

	if( recRoute != 0 )
	{
		export_fce->fce_actionTux(tux, recRoute);
	}

	destroyListItem(listFork, destroyAlternative);
	destroyListItem(listAlternative, destroyAlternative);
	destroyListItem(listDst, destroyAlternative);
/*
	printf("countFork = %d\n", countFork);
	printf("countDel = %d\n", countDel);
	printf("countLimit = %d\n", countLimit);
	printf("countDo = %d\n", countDo);
*/
}

static void action_tuxAI(space_t *space, tux_t *tux, void *p)
{
	if( tux->control == TUX_CONTROL_AI &&
	    tux->status == TUX_STATUS_ALIVE )
	{
		eventTuxAI(tux);
	}
}

int event()
{
	static my_time_t lastEvent = 0;
	my_time_t curentTime;
	arena_t *arena;
	int countTuxAI;
	//int i;

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

	if( arena == NULL )
	{
		return 0;
	}

	countTuxAI = 0;

	actionSpace(arena->spaceTux, action_tuxAI, NULL);
/*
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
*/
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

void recvMsg(char *msg)
{
}

int destroy()
{
	return 0;
}
