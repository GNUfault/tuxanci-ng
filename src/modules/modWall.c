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
#endif

#ifdef PUBLIC_SERVER
#include "publicServer.h"
#endif

typedef struct wall_struct
{
	int id;

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

static space_t *spaceWall;

#ifndef PUBLIC_SERVER	
static space_t *spaceImgWall;
#endif

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
	static int last_id = 0;
	wall_t *new;
	
#ifndef PUBLIC_SERVER	
	assert( img != NULL );
#endif
	new  = malloc( sizeof(wall_t) );
	assert( new != NULL );

	new->id = ++last_id;
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

void drawListWall(list_t *list)
{
	wall_t *thisWall;
	int i;

	assert( list != NULL );

	for( i = 0 ; i < list->count ; i++ )
	{
		thisWall  = (wall_t *)list->list[i];
		assert( thisWall != NULL );
		drawWall(thisWall);
	}
}

#endif

void eventConflictShotWithWall()
{
	arena_t *arena;
	shot_t *thisShot;
	tux_t *author;
	int i;

	arena = export_fce->fce_getCurrentArena();

	for( i = 0 ; i < arena->spaceShot->list->count ; i++ )
	{
		thisShot  = (shot_t *)arena->spaceShot->list->list[i];
		assert( thisShot != NULL );

		if( isConflictWithObjectFromSpace(spaceWall, thisShot->x, thisShot->y, thisShot->w, thisShot->h) )
		{
			author = getObjectFromSpaceWithID(
				export_fce->fce_getCurrentArena()->spaceTux, thisShot->author_id );
			
			if( author != NULL &&
			    author->bonus == BONUS_GHOST &&
			    author->bonus_time > 0 )
			{
				continue;
			}

			if( thisShot->gun == GUN_BOMBBALL)
			{
				if( export_fce->fce_getNetTypeGame() != NET_GAME_TYPE_CLIENT )
				{
					export_fce->fce_boundBombBall(thisShot);
				}

				continue;
			}

			delObjectFromSpaceWithObject(export_fce->fce_getCurrentArena()->spaceShot,
				thisShot, export_fce->fce_destroyShot);
			
			//delListItem(listShot, i, export_fce->fce_destroyShot);
			i--;
		}
	}
}

void destroyWall(wall_t *p)
{
	assert( p != NULL );
	free(p);
}

static void getStatusWall(void *p, int *id, int *x, int *y, int *w, int *h)
{
	wall_t *wall;

	wall = p;

	*id = wall->id;
	*x = wall->x;
	*y = wall->y;
	*w = wall->w;
	*h = wall->h;
}

static void setStatusWall(void *p, int x, int y, int w, int h)
{
	wall_t *wall;

	wall = p;

	wall->x = x;
	wall->y = y;
	wall->w = w;
	wall->h = h;
}

#ifndef PUBLIC_SERVER	

static void getStatusImgWall(void *p, int *id, int *x, int *y, int *w, int *h)
{
	wall_t *wall;

	wall = p;

	*id = wall->id;
	*x = wall->img_x;
	*y = wall->img_y;
	*w = wall->img->w;
	*h = wall->img->h;
}

static void setStatusImgWall(void *p, int x, int y, int w, int h)
{
}

#endif

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
	char str_rel[STR_SIZE];
	int x, y, w, h, img_x, img_y, layer;
	int rel;
	wall_t *new;

	rel = 0;

	if( export_fce->fce_getValue(line, "rel", str_rel, STR_NUM_SIZE) != 0 )strcpy(str_rel, "0");
	if( export_fce->fce_getValue(line, "x", str_x, STR_NUM_SIZE) != 0 )return;
	if( export_fce->fce_getValue(line, "y", str_y, STR_NUM_SIZE) != 0 )return;
	if( export_fce->fce_getValue(line, "w", str_w, STR_NUM_SIZE) != 0 )return;
	if( export_fce->fce_getValue(line, "h", str_h, STR_NUM_SIZE) != 0 )return;
	if( export_fce->fce_getValue(line, "img_x", str_img_x, STR_NUM_SIZE) != 0 )return;
	if( export_fce->fce_getValue(line, "img_y", str_img_y, STR_NUM_SIZE) != 0 )return;
	if( export_fce->fce_getValue(line, "layer", str_layer, STR_NUM_SIZE) != 0 )return;
	if( export_fce->fce_getValue(line, "image", str_image, STR_SIZE) != 0 )return;

	rel = atoi(str_rel);
	x  = atoi(str_x);
	y  = atoi(str_y);
	w  = atoi(str_w);
	h  = atoi(str_h);
	img_x  = atoi(str_img_x);
	img_y  = atoi(str_img_y);
	layer  = atoi(str_layer);

	if( rel == 1 )
	{
		img_x += x;
		img_y += y;
	}

#ifndef PUBLIC_SERVER
	new = newWall(x, y, w, h, img_x, img_y, layer, export_fce->fce_getImage(IMAGE_GROUP_USER, str_image) );
#endif

#ifdef PUBLIC_SERVER
	new = newWall(x, y, w, h, img_x, img_y, layer);
#endif

	if( spaceWall == NULL )
	{
		spaceWall  = newSpace(export_fce->fce_getCurrentArena()->w, export_fce->fce_getCurrentArena()->h,
				320, 240, getStatusWall, setStatusWall);

#ifndef PUBLIC_SERVER	
		spaceImgWall  = newSpace(export_fce->fce_getCurrentArena()->w, export_fce->fce_getCurrentArena()->h,
				320, 240, getStatusImgWall, setStatusImgWall);
#endif
	}

	addObjectToSpace(spaceWall, new);

#ifndef PUBLIC_SERVER	
	addObjectToSpace(spaceImgWall, new);
#endif
}

int init(export_fce_t *p)
{
	export_fce = p;
	listWall = newList();

	return 0;
}

#ifndef PUBLIC_SERVER

int draw(int x, int y, int w, int h)
{
	wall_t *thisWall;
	int i;

	if( spaceWall == NULL )
	{
		return 0;
	}

	listDoEmpty(listWall);
	getObjectFromSpace(spaceImgWall, x, y, w, h, listWall);

	for( i = 0 ; i < listWall->count ; i++ )
	{
		thisWall  = (wall_t *)listWall->list[i];
		assert( thisWall != NULL );
		drawWall(thisWall);
	}

	//printSpace(spaceWall);

	return 0;
}
#endif


int event()
{
	if( spaceWall == NULL )
	{
		return 0;
	}

	eventConflictShotWithWall(export_fce->fce_getCurrentArena()->spaceShot->list);
	return 0;
}

int isConflict(int x, int y, int w, int h)
{
	if( spaceWall == NULL )
	{
		return 0;
	}

	return isConflictWithObjectFromSpace(spaceWall, x, y, w, h);
}

void cmdArena(char *line)
{
	if( strncmp(line, "wall", 4) == 0 )cmd_wall(line);
}

int destroy()
{
	destroySpaceWithObject(spaceWall, destroyWall);
#ifndef PUBLIC_SERVER	
	destroySpace(spaceImgWall);
#endif
	destroyList(listWall);
	return 0;
}
