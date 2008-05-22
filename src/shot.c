
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "main.h"
#include "shot.h"
#include "gun.h"
#include "wall.h"
#include "pipe.h"
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
static list_t *listID;

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

	listID = newListID();
	
	isShotInit = TRUE;
}

shot_t* newShot(int x,int y, int px, int py, int gun, int author_id)
{
	shot_t *new;
	tux_t *author;

	new = malloc( sizeof(shot_t) );
	memset(new, 0, sizeof(shot_t) );

	new->id = getNewID(listID);
	new->x = x;
	new->y = y;
	new->px = px;
	new->py = py;
	new->gun = gun;
	new->author_id = author_id;

	new->position = POSITION_UNKNOWN;

	author = getTuxID(getCurrentArena()->listTux, author_id);

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

shot_t* getShotID(list_t *listShot, int id)
{
	shot_t *thisShot;
	int i;

	assert( listShot != NULL );

	for( i = 0 ; i < listShot->count ; i++ )
	{
		thisShot  = (shot_t *)listShot->list[i];
		assert( thisShot != NULL );

		if( thisShot->id == id )
		{
			return thisShot;
		}
	}

	return NULL;
}

void replaceShotID(shot_t *shot, int id)
{
	replaceID(listID, shot->id, id);
	shot->id = id;
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

int isConflictWithListShot(list_t *listShot, int x, int y, int w, int h)
{
	shot_t *thisShot;
	int i;

	assert( listShot != NULL );

	for( i = 0 ; i < listShot->count ; i++ )
	{
		thisShot  = (shot_t *)listShot->list[i];
		assert( thisShot != NULL );

		if( conflictSpace(x, y, w, h,
		    thisShot->x, thisShot->y, thisShot->w, thisShot->h) )
		{
			return 1;
		}
	}

	return 0;
}

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

void eventMoveListShot(list_t *listShot)
{
	shot_t *thisShot;
	int i;

	assert( listShot != NULL );

	for( i = 0 ; i < listShot->count ; i++ )
	{
		thisShot  = (shot_t *)listShot->list[i];
		assert( thisShot != NULL );

		thisShot->x += thisShot->px;
		thisShot->y += thisShot->py;
/*
		if( thisShot->gun == GUN_BOMBBALL && (
		    isConflictWithListWall(getCurrentArena()->listWall, thisShot->x, thisShot->y, thisShot->w, thisShot->h) ||
		    isConflictWithListPipe(getCurrentArena()->listPipe, thisShot->x, thisShot->y, thisShot->w, thisShot->h) ) )
		{
			if( getNetTypeGame() != NET_GAME_TYPE_CLIENT )
			{
				boundBombBall(thisShot);
			}
			else
			{
				delListItem(listShot, i, destroyShot);
				i--;
				continue;
			}
		}
*/
		if( thisShot->x+thisShot->w < 0 || thisShot->x > WINDOW_SIZE_X ||
		    thisShot->y+thisShot->h < 0 || thisShot->y > WINDOW_SIZE_Y )
		{
			delListItem(listShot, i, destroyShot);
			i--;
			continue;
		}
	}
}

/*
static int getSppedShot(shot_t *shot)
{
	return ( myAbs(shot->px) > myAbs(shot->py) ? myAbs(shot->px) : myAbs(shot->py) );
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
		transformOnlyLasser(shot);
	}
}

void moveShot(shot_t *shot, int position, int src_x, int src_y,
	int dist_x, int dist_y, int dist_w, int dist_h)
{
	int offset = 0;

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
			shot->x = dist_x + offset;
			shot->y = dist_y;
		break;

		case TUX_LEFT :
			shot->x = dist_x;
			shot->y = dist_y + offset;
		break;

		case TUX_RIGHT :
			shot->x = dist_x + dist_w;
			shot->y = dist_y + offset;
		break;

		case TUX_DOWN :
			shot->x = dist_x + offset;
			shot->y = dist_y + dist_h;
		break;
	}

	shot->x += shot->px;
	shot->y += shot->py;

	if( getNetTypeGame() == NET_GAME_TYPE_SERVER )
	{
		proto_send_shot_server(PROTO_SEND_ALL, NULL, shot);
	}

}
*/
void destroyShot(shot_t *p)
{
	assert( p != NULL );
	delID(listID, p->id);
	free(p);
}

void quitShot()
{
	destroyListID(listID);
	isShotInit = FALSE;
}

