
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
#    include "interface.h"
#    include "image.h"
#endif

#ifdef PUBLIC_SERVER
#    include "publicServer.h"
#endif

typedef struct wall_struct {
	int id;

	int x;						// poloha steny  
	int y;

	int w;						// rozmery steny
	int h;

	int img_x;					// poloha obrazka
	int img_y;

	int layer;					// vrstva

#ifndef PUBLIC_SERVER
	image_t *img;				//obrazok
#endif
} wall_t;

static export_fce_t *export_fce;

static space_t *spaceWall;

#ifndef PUBLIC_SERVER
static space_t *spaceImgWall;
#endif

static list_t *listWall;

#ifndef PUBLIC_SERVER
wall_t *newWall(int x, int y, int w, int h,
				int img_x, int img_y, int layer, image_t * img)
#endif
#ifdef PUBLIC_SERVER
	wall_t *newWall(int x, int y, int w, int h,
					int img_x, int img_y, int layer)
#endif
{
	static int last_id = 0;
	wall_t *new;

#ifndef PUBLIC_SERVER
	assert(img != NULL);
#endif
	new = malloc(sizeof(wall_t));
	assert(new != NULL);

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

void drawWall(wall_t * p)
{
	assert(p != NULL);

	export_fce->fce_addLayer(p->img, p->img_x, p->img_y, 0, 0, p->img->w,
							 p->img->h, p->layer);
}

void drawListWall(list_t * list)
{
	wall_t *thisWall;
	int i;

	assert(list != NULL);

	for (i = 0; i < list->count; i++) {
		thisWall = (wall_t *) list->list[i];
		assert(thisWall != NULL);
		drawWall(thisWall);
	}
}

#endif

void destroyWall(wall_t * p)
{
	assert(p != NULL);
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
	UNUSET(p);
	UNUSET(x);
	UNUSET(y);
	UNUSET(w);
	UNUSET(h);
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

	if (export_fce->fce_getValue(line, "rel", str_rel, STR_NUM_SIZE) != 0)
		strcpy(str_rel, "0");
	if (export_fce->fce_getValue(line, "x", str_x, STR_NUM_SIZE) != 0)
		return;
	if (export_fce->fce_getValue(line, "y", str_y, STR_NUM_SIZE) != 0)
		return;
	if (export_fce->fce_getValue(line, "w", str_w, STR_NUM_SIZE) != 0)
		return;
	if (export_fce->fce_getValue(line, "h", str_h, STR_NUM_SIZE) != 0)
		return;
	if (export_fce->fce_getValue(line, "img_x", str_img_x, STR_NUM_SIZE) != 0)
		return;
	if (export_fce->fce_getValue(line, "img_y", str_img_y, STR_NUM_SIZE) != 0)
		return;
	if (export_fce->fce_getValue(line, "layer", str_layer, STR_NUM_SIZE) != 0)
		return;
	if (export_fce->fce_getValue(line, "image", str_image, STR_SIZE) != 0)
		return;

	rel = atoi(str_rel);
	x = atoi(str_x);
	y = atoi(str_y);
	w = atoi(str_w);
	h = atoi(str_h);
	img_x = atoi(str_img_x);
	img_y = atoi(str_img_y);
	layer = atoi(str_layer);

	if (rel == 1) {
		img_x += x;
		img_y += y;
	}
#ifndef PUBLIC_SERVER
	new =
		newWall(x, y, w, h, img_x, img_y, layer,
				export_fce->fce_image_get(IMAGE_GROUP_USER, str_image));
#endif

#ifdef PUBLIC_SERVER
	new = newWall(x, y, w, h, img_x, img_y, layer);
#endif

	if (spaceWall == NULL) {
		spaceWall =
			space_new(export_fce->fce_arena_get_current()->w,
					 export_fce->fce_arena_get_current()->h, 320, 240,
					 getStatusWall, setStatusWall);

#ifndef PUBLIC_SERVER
		spaceImgWall =
			space_new(export_fce->fce_arena_get_current()->w,
					 export_fce->fce_arena_get_current()->h, 320, 240,
					 getStatusImgWall, setStatusImgWall);
#endif
	}

	space_add(spaceWall, new);

#ifndef PUBLIC_SERVER
	space_add(spaceImgWall, new);
#endif
}

int init(export_fce_t * p)
{
	export_fce = p;
	listWall = list_new();

	return 0;
}

#ifndef PUBLIC_SERVER

static void action_drawwall(space_t * space, wall_t * wall, void *p)
{
	UNUSET(space);
	UNUSET(p);

	drawWall(wall);
}

int draw(int x, int y, int w, int h)
{
	if (spaceWall == NULL) {
		return 0;
	}

	space_action_from_location(spaceImgWall, action_drawwall, NULL, x, y, w, h);

	//space_print(spaceWall);

	return 0;
}
#endif

static void action_eventwall(space_t * space, wall_t * wall, shot_t * shot)
{
	arena_t *arena;
	tux_t *author;

	UNUSET(space);
	UNUSET(wall);

	arena = export_fce->fce_arena_get_current();

	author = space_get_object_id(arena->spaceTux, shot->author_id);

	if (author != NULL &&
		author->bonus == BONUS_GHOST && author->bonus_time > 0) {
		return;
	}

	if (shot->gun == GUN_BOMBBALL) {
		if (export_fce->fce_net_multiplayer_get_game_type() != NET_GAME_TYPE_CLIENT) {
			export_fce->fce_shot_bound_bombBall(shot);
		}

		return;
	}
	//space_del_with_item(arena->spaceShot, shot, export_fce->fce_shot_destroy);
	shot->del = TRUE;
}

static void
action_eventshot(space_t * space, shot_t * shot, space_t * p_spaceWall)
{
	space_action_from_location(p_spaceWall, action_eventwall, shot, shot->x,
							shot->y, shot->w, shot->h);

	if (shot->del == TRUE) {
		space_del_with_item(space, shot, export_fce->fce_shot_destroy);
	}
}

int event()
{
	if (spaceWall == NULL) {
		return 0;
	}

	space_action(export_fce->fce_arena_get_current()->spaceShot,
				action_eventshot, spaceWall);

	return 0;
}

int isConflict(int x, int y, int w, int h)
{
	if (spaceWall == NULL) {
		return 0;
	}

	return space_is_conflict_with_object(spaceWall, x, y, w, h);
}

void cmdArena(char *line)
{
	if (strncmp(line, "wall", 4) == 0)
		cmd_wall(line);
}

void recvMsg(char *msg)
{
	UNUSET(msg);
}

int destroy()
{
	space_destroy_with_item(spaceWall, destroyWall);
#ifndef PUBLIC_SERVER
	space_destroy(spaceImgWall);
#endif
	list_destroy(listWall);
	return 0;
}
