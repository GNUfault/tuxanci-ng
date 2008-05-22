
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "main.h"
#include "interface.h"
#include "modules.h"
#include "tux.h"
#include "shot.h"
#include "list.h"
#include "gun.h"
#include "image.h"

#ifdef PUBLIC_SERVER
#include "publicServer.h"
#endif

typedef struct wall_struct
{
	int x; // poloha steny	
	int y;

	int w; // rozmery steny
	int h;

	int img_x; // poloha obrazka
	int img_y;

	int layer; // vrstva

#ifndef PUBLIC_SERVER	
	SDL_Surface *img; //obrazok
#endif
} wall_t;

static export_fce_t *export_fce;

static list_t *listWall;

#ifndef PUBLIC_SERVER	
wall_t* newWall(int x, int y, int w, int h,
	int img_x, int img_y, int layer, SDL_Surface *img)
#endif

#ifdef PUBLIC_SERVER	
wall_t* newWall(int x, int y, int w, int h,
	int img_x, int img_y, int layer)
#endif
{
	wall_t *new;
	
#ifndef PUBLIC_SERVER	
	assert( img != NULL );
#endif
	new  = malloc( sizeof(wall_t) );
	assert( new != NULL );

	new->x = x;
	new->y = y;
	new->w = w;
	new->h = h;
	new->img_x = img_x;
	new->img_y = img_y;
	new->layer = layer;
#ifndef PUBLIC_SERVER
	new->img = img;
#endif

	return new;
}

#ifndef PUBLIC_SERVER	

void drawWall(wall_t *p)
{
	assert( p != NULL );

	export_fce->fce_addLayer(p->img, p->img_x, p->img_y, 0, 0, p->img->w, p->img->h, p->layer);
}

void drawListWall(list_t *listWall)
{
	wall_t *thisWall;
	int i;

	assert( listWall != NULL );

	for( i = 0 ; i < listWall->count ; i++ )
	{
		thisWall  = (wall_t *)listWall->list[i];
		assert( thisWall != NULL );
		drawWall(thisWall);
	}
}

#endif

wall_t* isConflictWithListWall(list_t *listWall, int x, int y, int w, int h)
{
	wall_t *thisWall;
	int i;

	assert( listWall != NULL );

	for( i = 0 ; i < listWall->count ; i++ )
	{
		thisWall  = (wall_t *)listWall->list[i];
		assert( thisWall != NULL );

		if( export_fce->fce_conflictSpace(x, y, w, h,
		    thisWall->x, thisWall->y, thisWall->w, thisWall->h) )
		{
			return thisWall;
		}
	}

	return NULL;
}

void eventConflictShotWithWall(list_t *listShot)
{
	shot_t *thisShot;
	tux_t *author;
	int i;

	assert( listWall != NULL );
	assert( listShot != NULL );
	
	for( i = 0 ; i < listShot->count ; i++ )
	{
		thisShot  = (shot_t *)listShot->list[i];
		assert( thisShot != NULL );

		if( isConflictWithListWall(listWall, thisShot->x, thisShot->y, thisShot->w, thisShot->h) != NULL )
		{
			author = export_fce->fce_getTuxID( listShot, thisShot->author_id );
			
			if( author != NULL &&
			    author->bonus == BONUS_GHOST &&
			    author->bonus_time > 0 )
			{
				continue;
			}

			if( thisShot->gun == GUN_BOMBBALL && export_fce->fce_getNetTypeGame() != NET_GAME_TYPE_CLIENT )
			{
				export_fce->fce_boundBombBall(thisShot);
				continue;
			}

			delListItem(listShot, i, export_fce->fce_destroyShot);
			i--;
		}
	}
}

void destroyWall(wall_t *p)
{
	assert( p != NULL );
	free(p);
}

static void cmd_wall(char *line)
{
	char str_x[STR_NUM_SIZE];
	char str_y[STR_NUM_SIZE];
	char str_img_x[STR_NUM_SIZE];
	char str_img_y[STR_NUM_SIZE];
	char str_w[STR_NUM_SIZE];
	char str_h[STR_NUM_SIZE];
	char str_layer[STR_NUM_SIZE];
	char str_image[STR_SIZE];
	wall_t *new;

	if( export_fce->fce_getValue(line, "x", str_x, STR_NUM_SIZE) != 0 )return;
	if( export_fce->fce_getValue(line, "y", str_y, STR_NUM_SIZE) != 0 )return;
	if( export_fce->fce_getValue(line, "w", str_w, STR_NUM_SIZE) != 0 )return;
	if( export_fce->fce_getValue(line, "h", str_h, STR_NUM_SIZE) != 0 )return;
	if( export_fce->fce_getValue(line, "img_x", str_img_x, STR_NUM_SIZE) != 0 )return;
	if( export_fce->fce_getValue(line, "img_y", str_img_y, STR_NUM_SIZE) != 0 )return;
	if( export_fce->fce_getValue(line, "layer", str_layer, STR_NUM_SIZE) != 0 )return;
	if( export_fce->fce_getValue(line, "image", str_image, STR_SIZE) != 0 )return;

#ifndef PUBLIC_SERVER
	new = newWall(atoi(str_x), atoi(str_y),
			atoi(str_w), atoi(str_h),
			atoi(str_img_x), atoi(str_img_y),
			atoi(str_layer), export_fce->fce_getImage(IMAGE_GROUP_USER, str_image) );
#endif

#ifdef PUBLIC_SERVER
	new = newWall(atoi(str_x), atoi(str_y),
			atoi(str_w), atoi(str_h),
			atoi(str_img_x), atoi(str_img_y),
			atoi(str_layer) );
#endif

	addList(listWall, new);
}

int init(export_fce_t *p)
{
	export_fce = p;

	listWall = newList();

	return 0;
}

#ifndef PUBLIC_SERVER

int draw()
{
	wall_t *thisWall;
	int i;

	assert( listWall != NULL );

	for( i = 0 ; i < listWall->count ; i++ )
	{
		thisWall  = (wall_t *)listWall->list[i];
		assert( thisWall != NULL );
		drawWall(thisWall);
	}

	return 0;
}
#endif


int event()
{
	eventConflictShotWithWall(export_fce->fce_getCurrentArena()->listShot);
	return 0;
}

int isConflict(int x, int y, int w, int h)
{
	if( isConflictWithListWall(listWall, x, y, w, h) != NULL )
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

void cmd(char *line)
{
	if( strncmp(line, "wall", 4) == 0 )cmd_wall(line);
}

int destroy()
{
	destroyListItem(listWall, destroyWall);

	return 0;
}
