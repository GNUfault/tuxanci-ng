
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "base/main.h"
#include "base/modules.h"
#include "base/tux.h"
#include "base/shot.h"
#include "base/list.h"
#include "base/gun.h"
#include "base/space.h"

#ifndef PUBLIC_SERVER
#include "client/interface.h"
#include "client/image.h"
#endif

#ifdef PUBLIC_SERVER
#include "server/publicServer.h"
#endif

typedef struct teleport_struct
{
	int id;

	int x; // poloha steny	
	int y;

	int w; // rozmery steny
	int h;

	int layer; // vrstva

#ifndef PUBLIC_SERVER	
	SDL_Surface *img; //obrazok
#endif
} teleport_t;

static export_fce_t *export_fce;

static space_t *spaceTeleport;
static list_t *listTeleport;

#ifndef PUBLIC_SERVER	
teleport_t* newTeleport(int x, int y, int w, int h, int layer, SDL_Surface *img)
#endif

#ifdef PUBLIC_SERVER	
teleport_t* newTeleport(int x, int y, int w, int h, int layer)
#endif
{
	static int last_id = 0;

	teleport_t *new;
	
#ifndef PUBLIC_SERVER	
	assert( img != NULL );
#endif

	new  = malloc( sizeof(teleport_t) );
	assert( new != NULL );

	new->id = ++last_id;
	new->x = x;
	new->y = y;
	new->w = w;
	new->h = h;
	new->layer = layer;
#ifndef PUBLIC_SERVER	
	new->img = img;
#endif

	return new;
}

static void setStatusTeleport(void *p, int x, int y, int w, int h)
{
	teleport_t *teleport;

	teleport = p;

	teleport->x = x;
	teleport->y = y;
	teleport->w = w;
	teleport->h = h;
}

static void getStatusTeleport(void *p, int *id, int *x, int *y, int *w, int *h)
{
	teleport_t *teleport;
	teleport = p;

	*id = teleport->id;
	*x = teleport->x;
	*y = teleport->y;
	*w = teleport->w;
	*h = teleport->h;
}

#ifndef PUBLIC_SERVER

static void drawTeleport(teleport_t *p)
{
	assert( p != NULL );

	export_fce->fce_addLayer(p->img, p->x, p->y, 0, 0, p->img->w, p->img->h, p->layer);
}

#endif

static void destroyTeleport(teleport_t *p)
{
	assert( p != NULL );
	free(p);
}


static void cmd_teleport(char *line)
{
	char str_x[STR_NUM_SIZE];
	char str_y[STR_NUM_SIZE];
	char str_w[STR_NUM_SIZE];
	char str_h[STR_NUM_SIZE];
	char str_layer[STR_NUM_SIZE];
	char str_image[STR_SIZE];
	teleport_t *new;

	if( export_fce->fce_getValue(line, "x", str_x, STR_NUM_SIZE) != 0 )return;
	if( export_fce->fce_getValue(line, "y", str_y, STR_NUM_SIZE) != 0 )return;
	if( export_fce->fce_getValue(line, "w", str_w, STR_NUM_SIZE) != 0 )return;
	if( export_fce->fce_getValue(line, "h", str_h, STR_NUM_SIZE) != 0 )return;
	if( export_fce->fce_getValue(line, "layer", str_layer, STR_NUM_SIZE) != 0 )return;
	if( export_fce->fce_getValue(line, "image", str_image, STR_SIZE) != 0 )return;

#ifndef PUBLIC_SERVER
	new = newTeleport(atoi(str_x), atoi(str_y),
			atoi(str_w), atoi(str_h),
			atoi(str_layer), export_fce->fce_getImage(IMAGE_GROUP_USER, str_image) );
#endif

#ifdef PUBLIC_SERVER
	new = newTeleport(atoi(str_x), atoi(str_y),
			atoi(str_w), atoi(str_h),
			atoi(str_layer) );
#endif

	if( spaceTeleport == NULL )
	{
		spaceTeleport  = newSpace(export_fce->fce_getCurrentArena()->w, export_fce->fce_getCurrentArena()->h,
				320, 240, getStatusTeleport, setStatusTeleport);
	}

	addObjectToSpace(spaceTeleport, new);
}

static teleport_t* getRandomTeleportBut(list_t *list, teleport_t *teleport)
{
	int x;

	do{
		x = random() % list->count;
	}while( list->list[x] == teleport );

	return (teleport_t *) list->list[x];
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

static void teleportTux(tux_t *tux, teleport_t *teleport)
{
	teleport_t *distTeleport;
	int dist_x, dist_y;

	if( tux->bonus == BONUS_GHOST ||
	    export_fce->fce_getNetTypeGame() == NET_GAME_TYPE_CLIENT )
	{
		return;
	}

	distTeleport = getRandomTeleportBut(spaceTeleport->list, teleport);
	
	//export_fce->fce_getTuxProportion(tux, &current_x, &current_y, NULL, NULL);
	
	switch( tux->position )
	{
		case TUX_UP :
			dist_x = distTeleport->x + distTeleport->w/2;
			dist_y = distTeleport->y - ( TUX_HEIGHT + 20 );
		break;
		case TUX_LEFT :
			dist_x = distTeleport->x - ( TUX_WIDTH + 20 );
			dist_y = distTeleport->y + distTeleport->h/2;
		break;
	
		case TUX_RIGHT :
			dist_x = distTeleport->x + distTeleport->w + 20;
			dist_y = distTeleport->y + distTeleport->h/2;
		break;
	
		case TUX_DOWN :
			dist_x = distTeleport->x + distTeleport->w/2;
			dist_y = distTeleport->y + distTeleport->h + 20;
		break;
	
		default :
			assert( ! "premenna p->control ma zlu hodnotu !" );
		break;
	}
	
	if( export_fce->fce_isFreeSpace(export_fce->fce_getCurrentArena(), dist_x, dist_y, TUX_WIDTH, TUX_HEIGHT) )
	{
		moveObjectInSpace(export_fce->fce_getCurrentArena()->spaceTux, tux, dist_x, dist_y);
		//export_fce->fce_setTuxProportion(tux, dist_x, dist_y);
#ifndef PUBLIC_SERVER
		//playSound("teleport", SOUND_GROUP_BASE);
#endif
		if( export_fce->fce_getNetTypeGame() == NET_GAME_TYPE_SERVER )
		{
			export_fce->fce_proto_send_newtux_server(PROTO_SEND_ALL, NULL, tux);
		}
	}
}

static int myAbs(int n)
{
	return ( n > 0 ? n : -n );
}


static int getSppedShot(shot_t *shot)
{
	return ( myAbs(shot->px) > myAbs(shot->py) ? myAbs(shot->px) : myAbs(shot->py) );
}

/*
static void transformOnlyLasser(shot_t *shot)
{
	switch( shot->position )
	{
		case TUX_RIGHT :
		case TUX_LEFT :
			shot->w = GUN_LASSER_HORIZONTAL; 
			shot->h = GUN_SHOT_VERTICAL; 
#ifndef PUBLIC_SERVER	
			shot->img = g_shot_lasserX;
#endif
		break;
		case TUX_UP :
		case TUX_DOWN :
			shot->w = GUN_SHOT_VERTICAL; 
			shot->h = GUN_LASSER_HORIZONTAL; 
#ifndef PUBLIC_SERVER	
			shot->img = g_shot_lasserY;
#endif
		break;
	}
}
*/

static void transformShot(shot_t *shot, int position)
{
	int speed;

	speed = getSppedShot(shot);

	switch( position )
	{
		case TUX_UP :
			shot->px = 0;
			shot->py = -speed;
		break;

		case TUX_LEFT :
			shot->px = -speed;
			shot->py = 0;
		break;

		case TUX_RIGHT :
			shot->px = speed;
			shot->py = 0;
		break;

		case TUX_DOWN :
			shot->px = 0;
			shot->py = +speed;
		break;
	}

	shot->position = position;	
	shot->isCanKillAuthor = TRUE;

	if( shot->gun == GUN_LASSER )
	{
		export_fce->fce_transformOnlyLasser(shot);
	}
}

static void moveShot(shot_t *shot, int position, int src_x, int src_y,
	int dist_x, int dist_y, int dist_w, int dist_h)
{
	int offset = 0;
	int new_x, new_y;

	switch( shot->position )
	{
		case TUX_UP :
		case TUX_DOWN :
			offset = shot->x - src_x;
		break;

		case TUX_RIGHT :
		case TUX_LEFT :
			offset = shot->y - src_y;
		break;
	}

	transformShot(shot, position);

	switch( shot->position )
	{
		case TUX_UP :
			new_x = dist_x + offset;
			new_y = dist_y;
		break;

		case TUX_LEFT :
			new_x = dist_x;
			new_y = dist_y + offset;
		break;

		case TUX_RIGHT :
			new_x = dist_x + dist_w;
			new_y = dist_y + offset;
		break;

		case TUX_DOWN :
			new_x = dist_x + offset;
			new_y = dist_y + dist_h;
		break;
	}

	new_y += shot->px;
	new_y += shot->py;

	moveObjectInSpace(export_fce->fce_getCurrentArena()->spaceShot, shot, new_x, new_y);

	if( export_fce->fce_getNetTypeGame() == NET_GAME_TYPE_SERVER )
	{
		export_fce->fce_proto_send_shot_server(PROTO_SEND_ALL, NULL, shot);
	}
}

static void moveShotFromTeleport(shot_t *shot, teleport_t *teleport, list_t *list)
{
	teleport_t *distTeleport;

	distTeleport = getRandomTeleportBut(list, teleport);

	moveShot(shot, getRandomPosition(), teleport->x, teleport->y,
		distTeleport->x, distTeleport->y, distTeleport->w, distTeleport->h);
}

int init(export_fce_t *p)
{
	export_fce = p;

	listTeleport = newList();

	return 0;
}

#ifndef PUBLIC_SERVER
int draw(int x, int y, int w, int h)
{
	teleport_t *thisTeleport;
	int i;

	if( spaceTeleport == NULL )
	{
		return 0;
	}

	listDoEmpty(listTeleport);
	getObjectFromSpace(spaceTeleport, x, y, w, h, listTeleport);

	for( i = 0 ; i < listTeleport->count ; i++ )
	{
		thisTeleport  = (teleport_t *)listTeleport->list[i];
		assert( thisTeleport != NULL );
		drawTeleport(thisTeleport);
	}

	return 0;
}
#endif

int event()
{
	teleport_t *thisTeleport;
	arena_t *arena;
	int i;

	if( spaceTeleport == NULL )
	{
		return 0;
	}

	if( export_fce->fce_getNetTypeGame() == NET_GAME_TYPE_CLIENT )
	{
		return 0;
	}

	arena = export_fce->fce_getCurrentArena();

	for( i = 0 ; i < arena->spaceTux->list->count ; i++ )
	{
		tux_t *thisTux;
		int x, y, w, h;
		int j;

		thisTux = (tux_t *)arena->spaceTux->list->list[i];
		export_fce->fce_getTuxProportion(thisTux, &x, &y, &w, &h);

		listDoEmpty(listTeleport);
		getObjectFromSpace(spaceTeleport, x, y, w, h, listTeleport);

		for( j = 0 ; j < listTeleport->count ; j++ )
		{
			thisTeleport = (teleport_t *)listTeleport->list[j];
			teleportTux(thisTux, thisTeleport);
		}
	}
	
	for( i = 0 ; i < arena->spaceShot->list->count ; i++ )
	{
		shot_t *thisShot;
		int j;

		thisShot  = (shot_t *)arena->spaceShot->list->list[i];
		assert( thisShot != NULL );

		listDoEmpty(listTeleport);
		getObjectFromSpace(spaceTeleport, thisShot->x, thisShot->y, thisShot->w, thisShot->h, listTeleport);

		for( j = 0 ; j < listTeleport->count ; j++ )
		{
			tux_t *author;

			thisTeleport = (teleport_t *)listTeleport->list[j];
			author = getObjectFromSpaceWithID(arena->spaceTux, thisShot->author_id);
			
			if( author != NULL && 
			    author->bonus == BONUS_GHOST &&
			    author->bonus_time > 0 )
			{
				continue;
			}

			moveShotFromTeleport(thisShot, thisTeleport, spaceTeleport->list);
		}
	}

	return 0;
}

int isConflict(int x, int y, int w, int h)
{
	if( spaceTeleport == NULL )
	{
		return 0;
	}

	return 0;
}

void cmd(char *line)
{
	if( strncmp(line, "teleport", 8) == 0 )cmd_teleport(line);
}

int destroy()
{
	destroySpaceWithObject(spaceTeleport, destroyTeleport);
	destroyList(listTeleport);
	return 0;
}
