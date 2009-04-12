
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
#include "proto.h"
#include "serverSendMsg.h"

#ifndef PUBLIC_SERVER
#    include "interface.h"
#    include "image.h"
#endif

#ifdef PUBLIC_SERVER
#    include "publicServer.h"
#endif

typedef struct pipe_struct {
	int x;						// poloha steny  
	int y;

	int w;						// rozmery steny
	int h;

	int id;
	int id_out;
	int position;

	int layer;					// vrstva

#ifndef PUBLIC_SERVER
	image_t *img;				//obrazok
#endif
} pipe_t;

static void (*fce_move_shot) (shot_t * shot, int position, int src_x,
							  int src_y, int dist_x, int dist_y, int dist_w,
							  int dist_h);

static export_fce_t *export_fce;

static list_t *listPipe;
static space_t *spacePipe;

#ifndef PUBLIC_SERVER
static pipe_t *newPipe(int x, int y, int w, int h, int layer, int id,
					   int id_out, int position, image_t * img)
#endif
#ifdef PUBLIC_SERVER
	static pipe_t *newPipe(int x, int y, int w, int h, int layer, int id,
						   int id_out, int position)
#endif
{
	pipe_t *new;

#ifndef PUBLIC_SERVER
	assert(img != NULL);
#endif

	new = malloc(sizeof(pipe_t));
	assert(new != NULL);

	new->x = x;
	new->y = y;
	new->w = w;
	new->h = h;
	new->layer = layer;
	new->id = id;
	new->id_out = id_out;
	new->position = position;
#ifndef PUBLIC_SERVER
	new->img = img;
#endif
	return new;
}

static void setStatusPipe(void *p, int x, int y, int w, int h)
{
	pipe_t *pipe;

	pipe = p;

	pipe->x = x;
	pipe->y = y;
	pipe->w = w;
	pipe->h = h;
}

static void getStatusPipe(void *p, int *id, int *x, int *y, int *w, int *h)
{
	pipe_t *pipe;

	pipe = p;

	*id = pipe->id;
	*x = pipe->x;
	*y = pipe->y;
	*w = pipe->w;
	*h = pipe->h;
}

#ifndef PUBLIC_SERVER

static void drawPipe(pipe_t * p)
{
	assert(p != NULL);

	export_fce->fce_addLayer(p->img, p->x, p->y, 0, 0, p->img->w, p->img->h,
							 p->layer);
}

#endif

static void destroyPipe(pipe_t * p)
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
	char str_id[STR_NUM_SIZE];
	char str_id_out[STR_NUM_SIZE];
	char str_position[STR_NUM_SIZE];
	char str_layer[STR_NUM_SIZE];
	char str_image[STR_SIZE];
	pipe_t *new;

	if (export_fce->fce_getValue(line, "x", str_x, STR_NUM_SIZE) != 0)
		return;
	if (export_fce->fce_getValue(line, "y", str_y, STR_NUM_SIZE) != 0)
		return;
	if (export_fce->fce_getValue(line, "w", str_w, STR_NUM_SIZE) != 0)
		return;
	if (export_fce->fce_getValue(line, "h", str_h, STR_NUM_SIZE) != 0)
		return;
	if (export_fce->fce_getValue(line, "id", str_id, STR_NUM_SIZE) != 0)
		return;
	if (export_fce->fce_getValue(line, "id_out", str_id_out, STR_NUM_SIZE) !=
		0)
		return;
	if (export_fce->
		fce_getValue(line, "position", str_position, STR_NUM_SIZE) != 0)
		return;
	if (export_fce->fce_getValue(line, "layer", str_layer, STR_NUM_SIZE) != 0)
		return;
	if (export_fce->fce_getValue(line, "image", str_image, STR_SIZE) != 0)
		return;

#ifndef PUBLIC_SERVER
	new = newPipe(atoi(str_x), atoi(str_y),
				  atoi(str_w), atoi(str_h),
				  atoi(str_layer), atoi(str_id), atoi(str_id_out),
				  atoi(str_position),
				  export_fce->fce_image_get(IMAGE_GROUP_USER, str_image));
#endif

#ifdef PUBLIC_SERVER
	new = newPipe(atoi(str_x), atoi(str_y),
				  atoi(str_w), atoi(str_h),
				  atoi(str_layer), atoi(str_id), atoi(str_id_out),
				  atoi(str_position));
#endif

	if (spacePipe == NULL) {
		spacePipe =
			space_new(export_fce->fce_arena_get_current()->w,
					 export_fce->fce_arena_get_current()->h, 320, 240,
					 getStatusPipe, setStatusPipe);
	}

	space_add(spacePipe, new);
}

static void moveShotFromPipe(shot_t * shot, pipe_t * pipe)
{
	pipe_t *distPipe;

	distPipe = space_get_object_id(spacePipe, pipe->id_out);

	if (distPipe == NULL) {
		fprintf(stderr, "Pipe ID for pipe \"%d\" was not found\n", pipe->id);
		return;
	}

	fce_move_shot(shot, distPipe->position, pipe->x, pipe->y,
				  distPipe->x, distPipe->y, distPipe->w, distPipe->h);
}

int init(export_fce_t * p)
{
	export_fce = p;

	listPipe = list_new();

	if (export_fce->fce_module_load_dep("libmodMove") != 0) {
		return -1;
	}

	if ((fce_move_shot = export_fce->fce_shareFunction_get("move_shot")) == NULL) {
		return -1;
	}

	return 0;
}

#ifndef PUBLIC_SERVER

static void action_drawpipe(space_t * space, pipe_t * pipe, void *p)
{
	drawPipe(pipe);
}

int draw(int x, int y, int w, int h)
{
	if (spacePipe == NULL) {
		return 0;
	}

	space_action_from_location(spacePipe, action_drawpipe, NULL, x, y, w, h);
	//space_print(spacePipe);

	return 0;
}
#endif

static int negPosition(int n)
{
	switch (n) {
	case TUX_UP:
		return TUX_DOWN;

	case TUX_LEFT:
		return TUX_RIGHT;

	case TUX_RIGHT:
		return TUX_LEFT;

	case TUX_DOWN:
		return TUX_UP;

	default:
		assert(!"Tux is moving in another dimension maybe!");
		break;
	}

	return -1;					// no warnning
}


#if 0 //zaloha
static void action_eventpipe(space_t * space, pipe_t * pipe, shot_t * shot)
{
	arena_t *arena;
	tux_t *author;

	arena = export_fce->fce_arena_get_current();

	author = space_get_object_id(arena->spaceTux, shot->author_id);

	if (author != NULL &&
		author->bonus == BONUS_GHOST && author->bonus_time > 0) {
		return;
	}

	if (negPosition(shot->position) == pipe->position &&
		export_fce->fce_netMultiplayer_get_game_type() != NET_GAME_TYPE_CLIENT) {
		moveShotFromPipe(shot, pipe);
	} else {
		if (shot->gun == GUN_BOMBBALL &&
			export_fce->fce_netMultiplayer_get_game_type() != NET_GAME_TYPE_CLIENT) {
			export_fce->fce_shot_bound_bombBall(shot);
		} else {
			//space_del_with_item(arena->spaceShot, shot, export_fce->fce_shot_destroy);
			shot->del = TRUE;
		}
	}
}
#endif

static void action_eventpipe(space_t * space, pipe_t * pipe, shot_t * shot)
{
	arena_t *arena;
	tux_t *author;

	arena = export_fce->fce_arena_get_current();

	author = space_get_object_id(arena->spaceTux, shot->author_id);

	if (author != NULL &&
		author->bonus == BONUS_GHOST && author->bonus_time > 0) {
		return;
	}

	if (negPosition(shot->position) == pipe->position ) {
		moveShotFromPipe(shot, pipe);
	} else {
		if (shot->gun == GUN_BOMBBALL) {
			export_fce->fce_shot_bound_bombBall(shot);
		} else {
			shot->del = TRUE;
		}
	}
}

static void
action_eventshot(space_t * space, shot_t * shot, space_t * p_spacePipe)
{
	space_action_from_location(p_spacePipe, action_eventpipe, shot, shot->x,
							shot->y, shot->w, shot->h);

	if (shot->del == TRUE) {
		if (netMultiplayer_get_game_type() == NET_GAME_TYPE_SERVER) {
			proto_send_del_server(PROTO_SEND_ALL, NULL, shot->id);
		}

		space_del_with_item(space, shot, export_fce->fce_shot_destroy);
	}
}

int event()
{
	if (spacePipe == NULL) {
		return 0;
	}

	// na skusku
	if (export_fce->fce_netMultiplayer_get_game_type() == NET_GAME_TYPE_CLIENT) {
		return 0;
	}

	space_action(export_fce->fce_arena_get_current()->spaceShot,
				action_eventshot, spacePipe);

	return 0;
}

int isConflict(int x, int y, int w, int h)
{
	if (spacePipe == NULL) {
		return 0;
	}

	return space_is_conflict_with_object(spacePipe, x, y, w, h);
}

void cmdArena(char *line)
{
	if (strncmp(line, "pipe", 4) == 0)
		cmd_teleport(line);
}

void recvMsg(char *msg)
{

}

int destroy()
{
	space_destroy_with_item(spacePipe, destroyPipe);
	list_destroy(listPipe);

	return 0;
}
