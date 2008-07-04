
#include <stdio.h>
#include <assert.h>

#include "base/main.h"
#include "base/list.h"
#include "base/tux.h"
#include "base/arena.h"

#include "client/interface.h"
#include "client/image.h"
#include "client/radar.h"

static SDL_Surface *g_radar;
static list_t *listRadar;

typedef struct radar_item_struct
{
	int id;
	int x, y;
	int type;
} radar_item_t;

static radar_item_t* newRadarItem(int id, int x, int y, int type)
{
	radar_item_t *new;

	new = malloc( sizeof(radar_item_t) );
	new->id = id;
	new->x = x;
	new->y = y;
	new->type = type;

	return new;
}

static void destroyRadarItem(radar_item_t *p)
{
	assert( p != NULL );
	free(p);
}

void addToRadar(int id, int x, int y, int type)
{
	int i;

	for( i = 0 ; i < listRadar->count ; i++ )
	{
		radar_item_t *thisRadarItem;

		thisRadarItem = (radar_item_t *)listRadar->list[i];

		if( thisRadarItem->id == id )
		{
			thisRadarItem->x = x;
			thisRadarItem->y = y;
			thisRadarItem->type = type;
			return;
		}
	}

	addList(listRadar, newRadarItem(id, x, y, type) );
}

void delFromRadar(int id)
{
	int i;

	for( i = 0 ; i < listRadar->count ; i++ )
	{
		radar_item_t *thisRadarItem;

		thisRadarItem = (radar_item_t *)listRadar->list[i];

		if( thisRadarItem->id == id )
		{
			delListItem(listRadar, i, destroyRadarItem);
			return;
		}
	}
}

void initRadar()
{
	assert( isImageInicialized() == TRUE );
	assert( isInterfaceInicialized() == TRUE );

	g_radar = addImageData("radar.png", IMAGE_ALPHA, "radar", IMAGE_GROUP_USER);
	listRadar = newList();
}

void drawRadar(arena_t *arena)
{
	int i;

	drawImage(g_radar, RADAR_LOCATION_X, RADAR_LOCATION_Y, 0, 0, RADAR_SIZE_X+2, RADAR_SIZE_Y+2);

	for( i = 0 ; i < listRadar->count ; i++ )
	{
		radar_item_t *thisRadarItem;
		int x, y;

		thisRadarItem = (radar_item_t *)listRadar->list[i];

		x = ( ( (float)RADAR_SIZE_X) / ( (float)arena->w ) ) * ( (float)thisRadarItem->x );
		y = ( ( (float)RADAR_SIZE_Y) / ( (float)arena->h ) ) * ( (float)thisRadarItem->y );

		drawImage(g_radar,
			RADAR_LOCATION_X+1+x, RADAR_LOCATION_Y+1+y,
			2*thisRadarItem->type, RADAR_SIZE_Y+2, 2, 2);
	}
}

void quitRadar()
{
	destroyListItem(listRadar, destroyRadarItem);
}
