
#include <stdlib.h>
#include <assert.h>

#include "main.h"
#include "shot.h"
#include "layer.h"
#include "image.h"
#include "gun.h"
#include "screen_world.h"

static SDL_Surface *g_shot_simple;
static SDL_Surface *g_shot_lasserX;
static SDL_Surface *g_shot_lasserY;

static bool_t isShotInit = FALSE;

bool_t isShotInicialized()
{
	return isShotInit;
}

void initShot()
{
	assert( isImageInicialized() == TRUE );

	g_shot_simple = addImageData("shot.png", IMAGE_ALPHA, "shot", IMAGE_GROUP_BASE);
	g_shot_lasserX = addImageData("lasserX.png", IMAGE_NO_ALPHA, "lasserX", IMAGE_GROUP_BASE);
	g_shot_lasserY = addImageData("lasserY.png", IMAGE_NO_ALPHA, "lasserY", IMAGE_GROUP_BASE);

	isShotInit = TRUE;
}

shot_t* newShot(int x,int y, int px, int py, int gun, tux_t *author)
{
	shot_t *new;
	static int last_id = 0;

	assert( author != NULL );

	new = malloc( sizeof(shot_t) );
	memset(new, 0, sizeof(shot_t) );

	new->id = ++last_id;
	new->x = x;
	new->y = y;
	new->px = px;
	new->py = py;
	new->gun = gun;
	new->author = author;
	new->position = author->position;
	new->isCanKillAuthor = FALSE;

	switch( gun )
	{
		case GUN_SIMPLE :
			new->w = GUN_SHOT_WIDTH; 
			new->h = GUN_SHOT_HEIGHT; 
			new->img = g_shot_simple;
		break;

		case GUN_LASSER :
			switch( author-> position )
			{
				case TUX_RIGHT :
				case TUX_LEFT :
					new->w = GUN_LASSER_HORIZONTAL; 
					new->h = GUN_SHOT_VERTICAL; 
					new->img = g_shot_lasserX;
				break;
				case TUX_UP :
				case TUX_DOWN :
					new->w = GUN_SHOT_VERTICAL; 
					new->h = GUN_LASSER_HORIZONTAL; 
					new->img = g_shot_lasserY;
				break;
			}
		break;

		default :
			assert( ! "Premenna weapon ma zlu hodnotu !" );
		break;
	}

	return new;
}

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

		if( thisShot->x+thisShot->w < 0 || thisShot->x > WINDOW_SIZE_X ||
		    thisShot->y+thisShot->h < 0 || thisShot->y > WINDOW_SIZE_Y )
		{
			delListItem(listShot, i, destroyShot);
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

static void eventOnlyLasser(shot_t *shot)
{
	switch( shot->position )
	{
		case TUX_RIGHT :
		case TUX_LEFT :
			shot->w = GUN_LASSER_HORIZONTAL; 
			shot->h = GUN_SHOT_VERTICAL; 
			shot->img = g_shot_lasserX;
		break;
		case TUX_UP :
		case TUX_DOWN :
			shot->w = GUN_SHOT_VERTICAL; 
			shot->h = GUN_LASSER_HORIZONTAL; 
			shot->img = g_shot_lasserY;
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
		eventOnlyLasser(shot);
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
}

void destroyShot(shot_t *p)
{
	assert( p != NULL );
	free(p);
}

void quitShot()
{
	isShotInit = FALSE;
}

