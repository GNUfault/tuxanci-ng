
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "main.h"
#include "list.h"
#include "tux.h"
#include "arena.h"

#include "interface.h"
#include "image.h"
#include "radar.h"

static image_t *g_radar;
static list_t *listRadar;

typedef struct radar_item_struct {
	int id;
	int x, y;
	int type;
} radar_item_t;

static radar_item_t *newRadarItem(int id, int x, int y, int type)
{
	radar_item_t *new;

	new = malloc(sizeof(radar_item_t));
	new->id = id;
	new->x = x;
	new->y = y;
	new->type = type;

	return new;
}

static void destroyRadarItem(radar_item_t * p)
{
	assert(p != NULL);
	free(p);
}

void radar_add(int id, int x, int y, int type)
{
	int i;

	for (i = 0; i < listRadar->count; i++) {
		radar_item_t *thisRadarItem;

		thisRadarItem = (radar_item_t *) listRadar->list[i];

		if (thisRadarItem->id == id) {
			thisRadarItem->x = x;
			thisRadarItem->y = y;
			thisRadarItem->type = type;
			return;
		}
	}

	list_add(listRadar, newRadarItem(id, x, y, type));
}

void radar_del(int id)
{
	int i;

	for (i = 0; i < listRadar->count; i++) {
		radar_item_t *thisRadarItem;

		thisRadarItem = (radar_item_t *) listRadar->list[i];

		if (thisRadarItem->id == id) {
			list_del_item(listRadar, i, destroyRadarItem);
			return;
		}
	}
}

void radar_init()
{
	assert(image_is_inicialized() == TRUE);
	assert(interface_is_inicialized() == TRUE);

	g_radar = image_add("radar.png", IMAGE_NO_ALPHA, "radar", IMAGE_GROUP_USER);
	listRadar = list_new();
}

void radar_draw(arena_t * arena)
{
	int i;

	image_draw(g_radar, RADAR_LOCATION_X, RADAR_LOCATION_Y, 0, 0, RADAR_SIZE_X + 2, RADAR_SIZE_Y + 2);

	for (i = 0; i < listRadar->count; i++) {
		radar_item_t *thisRadarItem;
		int x, y;

		thisRadarItem = (radar_item_t *) listRadar->list[i];

		x = (((float) RADAR_SIZE_X) / ((float) arena->w)) * ((float) thisRadarItem->x);

		y = (((float) RADAR_SIZE_Y) / ((float) arena->h)) * ((float) thisRadarItem->y);

		if (x >= 0 && x <= RADAR_SIZE_X && y >= 0 && y <= RADAR_SIZE_Y) {
			image_draw(g_radar, RADAR_LOCATION_X + 1 + x, RADAR_LOCATION_Y + 1 + y,
				  2 * thisRadarItem->type, RADAR_SIZE_Y + 2, 2, 2);
		}
	}
}

void radar_quit()
{
	list_destroy_item(listRadar, destroyRadarItem);
}
