
#include <stdlib.h>
#include <assert.h>

#include "main.h"
#include "tux.h"
#include "teleport.h"
#include "shot.h"

#ifndef BUBLIC_SERVER
#include "layer.h"
#include "screen_world.h"
#include "sound.h"
#endif

#ifdef BUBLIC_SERVER
#include "publicServer.h"
#endif

#ifndef BUBLIC_SERVER	
teleport_t* newTeleport(int x, int y, int w, int h, int layer, SDL_Surface *img)
#endif

#ifdef BUBLIC_SERVER	
teleport_t* newTeleport(int x, int y, int w, int h, int layer)
#endif
{
	teleport_t *new;
	
#ifndef BUBLIC_SERVER	
	assert( img != NULL );
#endif

	new  = malloc( sizeof(teleport_t) );
	assert( new != NULL );

	new->x = x;
	new->y = y;
	new->w = w;
	new->h = h;
	new->layer = layer;
#ifndef BUBLIC_SERVER	
	new->img = img;
#endif
	return new;
}

#ifndef BUBLIC_SERVER

void drawTeleport(teleport_t *p)
{
	assert( p != NULL );

	addLayer(p->img, p->x, p->y, 0, 0, p->img->w, p->img->h, p->layer);
}

void drawListTeleport(list_t *listTeleport)
{
	teleport_t *thisTeleport;
	int i;

	assert( listTeleport != NULL );

	for( i = 0 ; i < listTeleport->count ; i++ )
	{
		thisTeleport  = (teleport_t *)listTeleport->list[i];
		assert( thisTeleport != NULL );
		drawTeleport(thisTeleport);
	}
}

#endif

teleport_t* isConflictWithListTeleport(list_t *listTeleport, int x, int y, int w, int h)
{
	teleport_t *thisTeleport;
	int i;

	assert( listTeleport != NULL );

	for( i = 0 ; i < listTeleport->count ; i++ )
	{
		thisTeleport  = (teleport_t *)listTeleport->list[i];
		assert( thisTeleport != NULL );

		if( conflictSpace(x, y, w, h,
		    thisTeleport->x, thisTeleport->y, thisTeleport->w, thisTeleport->h) )
		{
			return thisTeleport;
		}
	}

	return NULL;
}

static teleport_t* getRandomTeleportBut(list_t *listTeleport, teleport_t *teleport)
{
	int x;

	do{
		x = random() % listTeleport->count;
	}while( listTeleport->list[x] == teleport );

	return (teleport_t *) listTeleport->list[x];
}

static int getRandomPosition()
{
	switch( random() % 4 )
	{
		case 0 :
		return TUX_UP;

		case 1 :
		return TUX_LEFT;

		case 2 :
		return TUX_RIGHT;

		case 3 :
		return TUX_DOWN;
	}

	assert( ! "Stupid error ! " );
}

static void moveShotFromTeleport(shot_t *shot, teleport_t *teleport, list_t *listTeleport)
{
	teleport_t *distTeleport;

	distTeleport = getRandomTeleportBut(listTeleport, teleport);

	moveShot(shot, getRandomPosition(), teleport->x, teleport->y,
		distTeleport->x, distTeleport->y, distTeleport->w, distTeleport->h);
}

void eventConflictShotWithTeleport(list_t *listTeleport, list_t *listShot)
{
	shot_t *thisShot;
	teleport_t *thisTeleport;
	int i;

	assert( listTeleport != NULL );
	assert( listShot != NULL );
	
	for( i = 0 ; i < listShot->count ; i++ )
	{
		thisShot  = (shot_t *)listShot->list[i];
		assert( thisShot != NULL );

		if( ( thisTeleport = isConflictWithListTeleport(listTeleport, thisShot->x, thisShot->y, thisShot->w, thisShot->h) ) != NULL )
		{
			if( thisShot->author->bonus == BONUS_GHOST &&
			    thisShot->author->bonus_time > 0 )
			{
				continue;
			}

			moveShotFromTeleport(thisShot, thisTeleport, listTeleport);
		}
	}
}

void eventTeleportTux(list_t *listTeleport, teleport_t *teleport, tux_t *tux)
{
	teleport_t *distTeleport;
	int current_x, current_y;
	int dist_x, dist_y;

	if( tux->bonus == BONUS_GHOST )
	{
		return;
	}

	distTeleport = getRandomTeleportBut(listTeleport, teleport);

	getTuxProportion(tux, &current_x, &current_y, NULL, NULL);

	switch( tux->position )
	{
		case TUX_UP :
			dist_x = distTeleport->x + distTeleport->w/2;
			dist_y = distTeleport->y - ( TUX_HEIGHT + 10 );
		break;

		case TUX_LEFT :
			dist_y = distTeleport->y + distTeleport->h/2;
			dist_x = distTeleport->x - ( TUX_WIDTH + 10 );
		break;

		case TUX_RIGHT :
			dist_y = distTeleport->y + distTeleport->h/2;
			dist_x = distTeleport->x + distTeleport->w + 10;
		break;

		case TUX_DOWN :
			dist_x = distTeleport->x + distTeleport->w/2;
			dist_y = distTeleport->y + distTeleport->h + 10;
		break;

		default :
			assert( ! "premenna p->control ma zlu hodnotu !" );
		break;
	}

	if( isFreeSpace(dist_x, dist_y, TUX_WIDTH, TUX_HEIGHT) )
	{
		setTuxProportion(tux, dist_x, dist_y);
#ifndef BUBLIC_SERVER
		playSound("teleport", SOUND_GROUP_BASE);
#endif
	}
}

void destroyTeleport(teleport_t *p)
{
	assert( p != NULL );
	free(p);
}

