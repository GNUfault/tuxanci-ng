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
#else /* PUBLIC_SERVER */
#include "publicServer.h"
#endif /* PUBLIC_SERVER */

typedef struct teleport_struct {
	int id;

	/* position of the teleport */
	int x;
	int y;

	/* size of the teleport */
	int w;
	int h;

	/* layer in the arena where the teleport lies */
	int layer;

#ifndef PUBLIC_SERVER
	/* its image */
	image_t *img;
#endif /* PUBLIC_SERVER */
} teleport_t;

static export_fce_t *export_fce;

static space_t *spaceTeleport;
static list_t *listTeleport;

static void (*fce_move_tux) (tux_t *tux, int x, int y, int w, int h);
static void (*fce_move_shot) (shot_t *shot, int position, int src_x,
							  int src_y, int dist_x, int dist_y, int dist_w,
							  int dist_h);

#ifndef PUBLIC_SERVER
static teleport_t *newTeleport(int x, int y, int w, int h, int layer, image_t *img)
#else /* PUBLIC_SERVER */
static teleport_t *newTeleport(int x, int y, int w, int h, int layer)
#endif /* PUBLIC_SERVER */
{
	static int last_id = 0;

	teleport_t *new;

#ifndef PUBLIC_SERVER
	assert(img != NULL);
#endif /* PUBLIC_SERVER */

	new = malloc(sizeof(teleport_t));
	assert(new != NULL);

	new->id = ++last_id;
	new->x = x;
	new->y = y;
	new->w = w;
	new->h = h;
	new->layer = layer;
#ifndef PUBLIC_SERVER
	new->img = img;
#endif /* PUBLIC_SERVER */

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
	assert(p != NULL);

	export_fce->fce_addLayer(p->img, p->x, p->y, 0, 0, p->img->w, p->img->h, p->layer);
}
#endif /* PUBLIC_SERVER */

static void destroyTeleport(teleport_t *p)
{
	assert(p != NULL);
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

	if (export_fce->fce_getValue(line, "x", str_x, STR_NUM_SIZE) != 0 ||
	    export_fce->fce_getValue(line, "y", str_y, STR_NUM_SIZE) != 0 ||
	    export_fce->fce_getValue(line, "w", str_w, STR_NUM_SIZE) != 0 ||
	    export_fce->fce_getValue(line, "h", str_h, STR_NUM_SIZE) != 0 ||
	    export_fce->fce_getValue(line, "layer", str_layer, STR_NUM_SIZE) != 0 ||
	    export_fce->fce_getValue(line, "image", str_image, STR_SIZE) != 0) {
		return;
	}

#ifndef PUBLIC_SERVER
	new = newTeleport(atoi(str_x), atoi(str_y),
			  atoi(str_w), atoi(str_h),
			  atoi(str_layer),
			  export_fce->fce_image_get(IMAGE_GROUP_USER, str_image));
#else /* PUBLIC_SERVER */
	new = newTeleport(atoi(str_x), atoi(str_y),
			  atoi(str_w), atoi(str_h),
			  atoi(str_layer));
#endif /* PUBLIC_SERVER */

	if (spaceTeleport == NULL) {
		spaceTeleport = space_new(export_fce->fce_arena_get_current()->w,
					  export_fce->fce_arena_get_current()->h,
					  320, 240,
					  getStatusTeleport, setStatusTeleport);
	}

	space_add(spaceTeleport, new);
}

static teleport_t *getRandomTeleportBut(space_t *space, teleport_t *teleport)
{
	int my_index;

	do {
		my_index = random() % space_get_count(space);
	} while (space_get_item(space, my_index) == teleport);

	return (teleport_t *) space_get_item(space, my_index);
}

static int getRandomPosition()
{
	switch (random() % 4) {
		case 0:
			return TUX_UP;

		case 1:
			return TUX_LEFT;

		case 2:
			return TUX_RIGHT;

		case 3:
			return TUX_DOWN;
	}

	fatal("Generated a random number which is not in range 0 to 3");

	return -1;
}

static void teleportTux(tux_t *tux, teleport_t *teleport)
{
	teleport_t *distTeleport;

	if (tux->bonus == BONUS_GHOST ||
	    export_fce->fce_net_multiplayer_get_game_type() == NET_GAME_TYPE_CLIENT) {
		return;
	}

	distTeleport = getRandomTeleportBut(spaceTeleport, teleport);

	fce_move_tux(tux, distTeleport->x, distTeleport->y, distTeleport->w, distTeleport->h);
}

static void moveShotFromTeleport(shot_t *shot, teleport_t *teleport, space_t *space)
{
	teleport_t *distTeleport;

	distTeleport = getRandomTeleportBut(space, teleport);

	fce_move_shot(shot, getRandomPosition(), teleport->x, teleport->y,
		      distTeleport->x, distTeleport->y, distTeleport->w,
		      distTeleport->h);
}

static int init(export_fce_t *p)
{
	export_fce = p;

	listTeleport = list_new();

	if (export_fce->fce_module_load_dep("libmodMove") != 0 ||
	    (fce_move_tux = export_fce->fce_share_function_get("move_tux")) == NULL ||
	    (fce_move_shot = export_fce->fce_share_function_get("move_shot")) == NULL) {
		return -1;
	}

	return 0;
}

#ifndef PUBLIC_SERVER
static void action_drawteleport(space_t *space, teleport_t *teleport, void *p)
{
	(void)space; (void)p;
	drawTeleport(teleport);
}

static int draw(int x, int y, int w, int h)
{
	if (spaceTeleport == NULL) {
		return 0;
	}

	space_action_from_location(spaceTeleport, action_drawteleport, NULL, x, y, w, h);

	return 0;
}
#endif /* PUBLIC_SERVER */

static void action_eventteleportshot(space_t *space, teleport_t *teleport, shot_t *shot)
{
	arena_t *arena;
	tux_t *author;
	(void)space;

	arena = export_fce->fce_arena_get_current();

	author = space_get_object_id(arena->spaceTux, shot->author_id);

	if (author != NULL && author->bonus == BONUS_GHOST && author->bonus_time > 0) {
		return;
	}

	moveShotFromTeleport(shot, teleport, spaceTeleport);
}

static void action_eventshot(space_t *space, shot_t *shot, space_t *p_spaceTeleport)
{
	(void)space;
	space_action_from_location(p_spaceTeleport, action_eventteleportshot,
				   shot, shot->x, shot->y, shot->w, shot->h);
}

static void action_eventteleporttux(space_t *space, teleport_t *teleport, tux_t *tux)
{
	(void)space;
	teleportTux(tux, teleport);
}

static void action_eventtux(space_t *space, tux_t *tux, space_t *p_spaceTeleport)
{
	(void)space;
	int x, y, w, h;

	export_fce->fce_tux_get_proportion(tux, &x, &y, &w, &h);

	space_action_from_location(p_spaceTeleport, action_eventteleporttux, tux, x, y, w, h);
}

static int event()
{
	if (spaceTeleport == NULL) {
		return 0;
	}

	if (export_fce->fce_net_multiplayer_get_game_type() == NET_GAME_TYPE_CLIENT) {
		return 0;
	}

	space_action(export_fce->fce_arena_get_current()->spaceShot, action_eventshot, spaceTeleport);
	space_action(export_fce->fce_arena_get_current()->spaceTux, action_eventtux, spaceTeleport);

	return 0;
}

static int isConflict(int x, int y, int w, int h)
{
	(void)x; (void)y; (void)w; (void)h;
	if (spaceTeleport == NULL) {
		return 0;
	}

	return 0;
}

static void cmdArena(char *line)
{
	if (strncmp(line, "teleport", 8) == 0) {
		cmd_teleport(line);
	}
}

static void recvMsg(char *msg)
{
	(void)msg;
}

static int destroy()
{
	space_destroy_with_item(spaceTeleport, destroyTeleport);
	spaceTeleport = NULL;

	list_destroy(listTeleport);
	listTeleport = NULL;

	return 0;
}

mod_sym_t modteleport_sym = { &init,
#ifndef PUBLIC_SERVER
			      &draw,
#else /* PUBLIC_SERVER */
			      0,
#endif /* PUBLIC_SERVER */
			      &event,
			      &isConflict,
			      &cmdArena,
			      &recvMsg,
			      &destroy };
