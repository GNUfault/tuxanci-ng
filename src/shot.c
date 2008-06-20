
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "main.h"
#include "shot.h"
#include "gun.h"
#include "net_multiplayer.h"
#include "proto.h"
#include "idManager.h"

#ifndef PUBLIC_SERVER
#include "image.h"
#include "screen_world.h"
#include "layer.h"
#endif

#ifdef PUBLIC_SERVER
#include "publicServer.h"
#endif

#ifndef PUBLIC_SERVER

static SDL_Surface *g_shot_simple;
static SDL_Surface *g_shot_lasserX;
static SDL_Surface *g_shot_lasserY;
static SDL_Surface *g_shot_bombball;

#endif

static bool_t isShotInit = FALSE;

bool_t isShotInicialized()
{
	return isShotInit;
}

void initShot()
{
#ifndef PUBLIC_SERVER
	assert( isImageInicialized() == TRUE );
#endif

#ifndef PUBLIC_SERVER

	g_shot_simple = addImageData("shot.png", IMAGE_ALPHA, "shot", IMAGE_GROUP_BASE);
	g_shot_lasserX = addImageData("lasserX.png", IMAGE_NO_ALPHA, "lasserX", IMAGE_GROUP_BASE);
	g_shot_lasserY = addImageData("lasserY.png", IMAGE_NO_ALPHA, "lasserY", IMAGE_GROUP_BASE);
	g_shot_bombball = addImageData("bombball.png", IMAGE_ALPHA, "bombball", IMAGE_GROUP_BASE);

#endif

	isShotInit = TRUE;
}

shot_t* newShot(int x,int y, int px, int py, int gun, int author_id)
{
	shot_t *new;
	tux_t *author;

	new = malloc( sizeof(shot_t) );
	memset(new, 0, sizeof(shot_t) );

	new->id = getNewID();
	new->x = x;
	new->y = y;
	new->px = px;
	new->py = py;
	new->gun = gun;
	new->author_id = author_id;

	new->position = POSITION_UNKNOWN;

	author = getObjectFromSpaceWithID(getCurrentArena()->spaceTux, author_id);

	if( author != NULL )
	{
		new->position = author->position;
	}

	new->isCanKillAuthor = FALSE;

	switch( gun )
	{
		case GUN_SIMPLE :
			new->w = GUN_SHOT_WIDTH; 
			new->h = GUN_SHOT_HEIGHT; 
#ifndef PUBLIC_SERVER	
			new->img = g_shot_simple;
#endif
		break;

		case GUN_LASSER :
			switch( author-> position )
			{
				case TUX_RIGHT :
				case TUX_LEFT :
					new->w = GUN_LASSER_HORIZONTAL; 
					new->h = GUN_SHOT_VERTICAL; 
#ifndef PUBLIC_SERVER	
					new->img = g_shot_lasserX;
#endif
				break;
				case TUX_UP :
				case TUX_DOWN :
					new->w = GUN_SHOT_VERTICAL; 
					new->h = GUN_LASSER_HORIZONTAL; 
#ifndef PUBLIC_SERVER	
					new->img = g_shot_lasserY;
#endif
				break;
			}
		break;

		case GUN_BOMBBALL :
			new->w = GUN_BOMBBALL_WIDTH; 
			new->h = GUN_BOMBBALL_HEIGHT; 

#ifndef PUBLIC_SERVER	
			new->img = g_shot_bombball;
#endif
		break;

		default :
			assert( ! "Premenna weapon ma zlu hodnotu !" );
		break;
	}

	return new;
}

void replaceShotID(shot_t *shot, int id)
{
	replaceID(shot->id, id);
	shot->id = id;
}


void getStatusShot(void *p, int *id, int *x,int *y, int *w, int *h)
{
	shot_t *shot;

	shot = p;
	*id = shot->id;
	*x = shot->x;
	*y = shot->y;
	*w = shot->w;
	*h = shot->h;
}

void setStatusShot(void *p, int x, int y, int w, int h)
{
	shot_t *shot;

	shot = p;
	shot->x = x;
	shot->y = y;
	shot->w = w;
	shot->h = h;
}

#ifndef PUBLIC_SERVER	

void drawShot(shot_t *p)
{
	assert( p != NULL );
	addLayer(p->img, p->x, p->y, 0, 0, p->img->w, p->img->h, TUX_LAYER);
}

void drawListShot(list_t *listShot)
{
	shot_t *thisShot;
	int i;

	assert( listShot != NULL );

	for( i = 0 ; i < listShot->count ; i++ )
	{
		thisShot  = (shot_t *)listShot->list[i];
		assert( thisShot != NULL );
		drawShot(thisShot);
	}
}

#endif

static int getRandomCourse(int x, int y)
{
	int ret;

	do{
		switch( random() % 3 )
		{
			case 0 :
				ret = y;
			break;
			case 1 :
				ret = -y;
			break;
			case 2 :
				ret = 0;
			break;
		}
	}while( ret == x );

	return ret;
}

void boundBombBall(shot_t *shot)
{
	if( shot->gun != GUN_BOMBBALL )
	{
		return;
	}

	shot->px  = getRandomCourse(shot->px, +10);
	shot->py  = getRandomCourse(shot->py, +10);
	shot->isCanKillAuthor = 1;

	if( getNetTypeGame() == NET_GAME_TYPE_SERVER )
	{
		proto_send_shot_server(PROTO_SEND_ALL, NULL, shot);
	}
}

void transformOnlyLasser(shot_t *shot)
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

void eventMoveListShot(arena_t *arena)
{
	shot_t *thisShot;
	int new_x, new_y;
	int i;

	for( i = 0 ; i < arena->spaceShot->list->count ; i++ )
	{
		thisShot  = (shot_t *) arena->spaceShot->list->list[i];
		assert( thisShot != NULL );

		new_x = thisShot->x + thisShot->px;
		new_y = thisShot->y + thisShot->py;

		moveObjectInSpace(getCurrentArena()->spaceShot, thisShot, new_x, new_y);

		if( thisShot->x+thisShot->w < 0 || thisShot->x > arena->w ||
		    thisShot->y+thisShot->h < 0 || thisShot->y > arena->h )
		{
			delObjectFromSpaceWithObject(arena->spaceShot,
				thisShot, destroyShot);
			i--;
			continue;
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


void checkShotIsInTuxScreen(arena_t *arena)
{
	int screen_x, screen_y;
	shot_t *thisShot;
	tux_t *thisTux;
	int speed;
	int i, j;

	if( getNetTypeGame() != NET_GAME_TYPE_SERVER )
	{
		return;
	}

	for( i = 0 ; i < arena->spaceTux->list->count ; i++ )
	{
		thisTux = (tux_t *)arena->spaceTux->list->list[i];
		speed = 25;

		if( thisTux->control != TUX_CONTROL_NET )
		{
			continue;
		}

		getCenterScreen(&screen_x, &screen_y, thisTux->x, thisTux->y);

		listDoEmpty(listHelp);
		getObjectFromSpace(arena->spaceShot, screen_x-speed, screen_y,  speed, WINDOW_SIZE_Y, listHelp);

		for( j = 0 ; j <listHelp->count ; j++ )
		{
			thisShot = (shot_t *)listHelp->list[j];
			client_t *thisClient;
			thisClient = getClientFromTux(thisTux);
			proto_send_shot_server(PROTO_SEND_ONE, thisClient, thisShot);
		}

		listDoEmpty(listHelp);
		getObjectFromSpace(arena->spaceShot, screen_x+WINDOW_SIZE_X, screen_y,  speed, WINDOW_SIZE_Y, listHelp);

		for( j = 0 ; j <listHelp->count ; j++ )
		{
			thisShot = (shot_t *)listHelp->list[j];
			client_t *thisClient;
			thisClient = getClientFromTux(thisTux);
			proto_send_shot_server(PROTO_SEND_ONE, thisClient, thisShot);
		}

		listDoEmpty(listHelp);
		getObjectFromSpace(arena->spaceShot, screen_x, screen_y-speed, WINDOW_SIZE_X, speed, listHelp);

		for( j = 0 ; j <listHelp->count ; j++ )
		{
			thisShot = (shot_t *)listHelp->list[j];
			client_t *thisClient;
			thisClient = getClientFromTux(thisTux);
			proto_send_shot_server(PROTO_SEND_ONE, thisClient, thisShot);
		}

		listDoEmpty(listHelp);
		getObjectFromSpace(arena->spaceShot, screen_x, screen_y+WINDOW_SIZE_Y, WINDOW_SIZE_X, speed, listHelp);

		for( j = 0 ; j <listHelp->count ; j++ )
		{
			thisShot = (shot_t *)listHelp->list[j];
			client_t *thisClient;
			thisClient = getClientFromTux(thisTux);
			proto_send_shot_server(PROTO_SEND_ONE, thisClient, thisShot);
		}
	}
}

void destroyShot(shot_t *p)
{
	assert( p != NULL );
	delID(p->id);
	free(p);
}

void quitShot()
{
	isShotInit = FALSE;
}
