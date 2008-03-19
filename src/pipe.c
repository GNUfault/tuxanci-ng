
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "main.h"
#include "pipe.h"
#include "shot.h"
#include "net_multiplayer.h"

#ifndef BUBLIC_SERVER
#include "screen_world.h"
#include "layer.h"
#endif

#ifdef BUBLIC_SERVER
#include "publicServer.h"
#endif

#ifndef BUBLIC_SERVER	
pipe_t* newPipe(int x, int y, int w, int h, int layer, int id, int id_out, int position, SDL_Surface *img)
#endif

#ifdef BUBLIC_SERVER	
pipe_t* newPipe(int x, int y, int w, int h, int layer, int id, int id_out, int position)
#endif

{
	pipe_t *new;
	
#ifndef BUBLIC_SERVER	
	assert( img != NULL );
#endif

	new  = malloc( sizeof(pipe_t) );
	assert( new != NULL );

	new->x = x;
	new->y = y;
	new->w = w;
	new->h = h;
	new->layer = layer;
	new->id = id;
	new->id_out = id_out;
	new->position = position;
#ifndef BUBLIC_SERVER	
	new->img = img;
#endif
	return new;
}

#ifndef BUBLIC_SERVER	

void drawPipe(pipe_t *p)
{
	assert( p != NULL );

	addLayer(p->img, p->x, p->y, 0, 0, p->img->w, p->img->h, p->layer);
}

void drawListPipe(list_t *listPipe)
{
	pipe_t *thisPipe;
	int i;

	assert( listPipe != NULL );

	for( i = 0 ; i < listPipe->count ; i++ )
	{
		thisPipe  = (pipe_t *)listPipe->list[i];
		assert( thisPipe != NULL );
		drawPipe(thisPipe);
	}
}

#endif

pipe_t* isConflictWithListPipe(list_t *listPipe, int x, int y, int w, int h)
{
	pipe_t *thisPipe;
	int i;

	assert( listPipe != NULL );

	for( i = 0 ; i < listPipe->count ; i++ )
	{
		thisPipe  = (pipe_t *)listPipe->list[i];
		assert( thisPipe != NULL );

		if( conflictSpace(x, y, w, h,
		    thisPipe->x, thisPipe->y, thisPipe->w, thisPipe->h) )
		{
			return thisPipe;
		}
	}

	return NULL;
}

static int negPosition(int n)
{
	switch( n )
	{
		case TUX_UP :
		return TUX_DOWN;

		case TUX_LEFT :
		return TUX_RIGHT;

		case TUX_RIGHT :
		return TUX_LEFT;

		case TUX_DOWN :
		return TUX_UP;

		default :
			assert( ! "premenna n ma zlu hodnotu !" );
		break;
	}
}

static pipe_t* getPipeId(list_t *listPipe, int id)
{
	int i;

	for( i = 0 ; i < listPipe->count ; i++ )
	{
		pipe_t *thisPipe;

		thisPipe = (pipe_t *) listPipe->list[i];

		if( thisPipe->id == id )
		{
			return thisPipe;
		}
	}

	return NULL;
}

static void moveShotFromPipe(shot_t *shot, pipe_t *pipe, list_t *listPipe)
{
	pipe_t *distPipe;

	distPipe = getPipeId(listPipe, pipe->id_out);

	if( distPipe == NULL )
	{
		fprintf(stderr, "Pipe %d ID not found\n", pipe->id);
		return;
	}

	moveShot(shot, distPipe->position, pipe->x, pipe->y,
		distPipe->x, distPipe->y, distPipe->w, distPipe->h);
}

void eventConflictShotWithPipe(list_t *listPipe, list_t *listShot)
{
	shot_t *thisShot;
	pipe_t *thisPipe;
	int i;

	assert( listPipe != NULL );
	assert( listShot != NULL );
	
	for( i = 0 ; i < listShot->count ; i++ )
	{
		thisShot  = (shot_t *)listShot->list[i];
		assert( thisShot != NULL );

		if( ( thisPipe = isConflictWithListPipe(listPipe, thisShot->x, thisShot->y,
			thisShot->w, thisShot->h) ) != NULL )
		{
			if( thisShot->author->bonus == BONUS_GHOST &&
			    thisShot->author->bonus_time > 0 )
			{
				continue;
			}

			if( negPosition( thisShot->position ) == thisPipe->position )
			{
				if( getNetTypeGame() == NET_GAME_TYPE_CLIENT )
				{
					delListItem(listShot, i, destroyShot);
					i--;
				}
				else
				{
					moveShotFromPipe(thisShot, thisPipe, listPipe);
				}
			}
			else
			{
				delListItem(listShot, i, destroyShot);
				i--;
			}
		}
	}
}

void destroyPipe(pipe_t *p)
{
	assert( p != NULL );
	free(p);
}

