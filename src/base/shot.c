#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "main.h"
#include "shot.h"
#include "gun.h"
#include "net_multiplayer.h"
#include "proto.h"
#include "idManager.h"
#include "serverSendMsg.h"

#ifndef PUBLIC_SERVER
#include "image.h"
#include "layer.h"
#include "world.h"
#else /* PUBLIC_SERVER */
#include "publicServer.h"
#endif /* PUBLIC_SERVER */

#ifndef PUBLIC_SERVER
static image_t *g_shot_simple;
static image_t *g_shot_lasserX;
static image_t *g_shot_lasserY;
static image_t *g_shot_bombball;
#endif /* PUBLIC_SERVER */

static bool_t isShotInit = FALSE;

bool_t shot_is_inicialized()
{
	return isShotInit;
}

void shot_init()
{
#ifndef PUBLIC_SERVER
	assert(image_is_inicialized() == TRUE);

	g_shot_simple = image_add("shot.png", IMAGE_ALPHA, "shot", IMAGE_GROUP_BASE);
	g_shot_lasserX = image_add("lasserX.png", IMAGE_NO_ALPHA, "lasserX", IMAGE_GROUP_BASE);
	g_shot_lasserY = image_add("lasserY.png", IMAGE_NO_ALPHA, "lasserY",IMAGE_GROUP_BASE);
	g_shot_bombball = image_add("bombball.png", IMAGE_ALPHA, "bombball", IMAGE_GROUP_BASE);
#endif /* PUBLIC_SERVER */

	isShotInit = TRUE;
}

shot_t *shot_new(int x, int y, int px, int py, int gun, int author_id)
{
	shot_t *new;
	tux_t *author;

	new = malloc(sizeof(shot_t));
	memset(new, 0, sizeof(shot_t));

	new->id = id_get_new();
	new->x = x;
	new->y = y;
	new->px = px;
	new->py = py;
	new->gun = gun;
	new->author_id = author_id;

	new->position = POSITION_UNKNOWN;

	author = space_get_object_id(arena_get_current()->spaceTux, author_id);

	if (author != NULL) {
		new->position = author->position;
	}

	new->isCanKillAuthor = FALSE;
	new->del = FALSE;

	switch (gun) {
		case GUN_SIMPLE:
			new->w = GUN_SHOT_WIDTH;
			new->h = GUN_SHOT_HEIGHT;
#ifndef PUBLIC_SERVER
			new->img = g_shot_simple;
#endif /* PUBLIC_SERVER */
			break;
	
		case GUN_LASSER:
			switch (author->position) {
				case TUX_RIGHT:
				case TUX_LEFT:
					new->w = GUN_LASSER_HORIZONTAL;
					new->h = GUN_SHOT_VERTICAL;
#ifndef PUBLIC_SERVER
					new->img = g_shot_lasserX;
#endif /* PUBLIC_SERVER */
					break;
				case TUX_UP:
				case TUX_DOWN:
					new->w = GUN_SHOT_VERTICAL;
					new->h = GUN_LASSER_HORIZONTAL;
#ifndef PUBLIC_SERVER
					new->img = g_shot_lasserY;
#endif /* PUBLIC_SERVER */
					break;
			}
			break;
	
		case GUN_BOMBBALL:
			new->w = GUN_BOMBBALL_WIDTH;
			new->h = GUN_BOMBBALL_HEIGHT;
	
#ifndef PUBLIC_SERVER
			new->img = g_shot_bombball;
#endif /* PUBLIC_SERVER */
			break;
	
		default:
			error("Unknown type of gun [%d]", gun);
			assert(0);
			break;
	}

	return new;
}

void shot_replace_id(shot_t *shot, int id)
{
	id_replace(shot->id, id);
	shot->id = id;
}


void shot_get_status(void *p, int *id, int *x, int *y, int *w, int *h)
{
	shot_t *shot;

	shot = p;
	*id = shot->id;
	*x = shot->x;
	*y = shot->y;
	*w = shot->w;
	*h = shot->h;
}

void shot_set_status(void *p, int x, int y, int w, int h)
{
	shot_t *shot;

	shot = p;
	shot->x = x;
	shot->y = y;
	shot->w = w;
	shot->h = h;
}

#ifndef PUBLIC_SERVER
void shot_draw(shot_t *p)
{
	assert(p != NULL);
	addLayer(p->img, p->x, p->y, 0, 0, p->img->w, p->img->h, TUX_LAYER);
}

void shot_draw_list(list_t *listShot)
{
	shot_t *thisShot;
	int i;

	assert(listShot != NULL);

	for (i = 0; i < listShot->count; i++) {
		thisShot = (shot_t *) listShot->list[i];
		assert(thisShot != NULL);
		shot_draw(thisShot);
	}
}
#endif /* PUBLIC_SERVER */

static int getRandomCourse(int x, int y)
{
	int ret = -1;

	do {
		switch (random() % 3) {
			case 0:
				ret = y;
				break;
			case 1:
				ret = -y;
				break;
			case 2:
				ret = 0;
				break;
		}
	} while (ret == x);

	return ret;
}

void shot_bound_bombBall(shot_t *shot)
{
	if (shot->gun != GUN_BOMBBALL) {
		return;
	}

	shot->px = getRandomCourse(shot->px, +10);
	shot->py = getRandomCourse(shot->py, +10);
	shot->isCanKillAuthor = 1;

	if (net_multiplayer_get_game_type() == NET_GAME_TYPE_SERVER) {
		proto_send_shot_server(PROTO_SEND_ALL, NULL, shot);
	}
}

void shot_transform_lasser(shot_t *shot)
{
	space_t *space;
	int mustRefesh = 0;

	space = arena_get_current()->spaceShot;

	if (space_get_object_id(space, shot->id) != NULL) {
		mustRefesh = 1;
	}

	if (mustRefesh) {
		space_del(space, shot);
	}

	switch (shot->position) {
		case TUX_RIGHT:
		case TUX_LEFT:
			shot->w = GUN_LASSER_HORIZONTAL;
			shot->h = GUN_SHOT_VERTICAL;
#ifndef PUBLIC_SERVER
			shot->img = g_shot_lasserX;
#endif /* PUBLIC_SERVER */
			break;

		case TUX_UP:
		case TUX_DOWN:
			shot->w = GUN_SHOT_VERTICAL;
			shot->h = GUN_LASSER_HORIZONTAL;
#ifndef PUBLIC_SERVER
			shot->img = g_shot_lasserY;
#endif /* PUBLIC_SERVER */
			break;
	}

	if (mustRefesh) {
		space_add(space, shot);
	}
}

static void action_moveShot(space_t *space, shot_t *shot, void *p)
{
	int new_x, new_y;
	arena_t *arena;
	(void)p;

	arena = arena_get_current();

	new_x = shot->x + shot->px;
	new_y = shot->y + shot->py;

	space_move_object(space, shot, new_x, new_y);

	if (shot->x + shot->w < 0 || shot->x > arena->w ||
	    shot->y + shot->h < 0 || shot->y > arena->h) {
		space_del_with_item(space, shot, shot_destroy);
	}
}

void shot_event_move_list(arena_t *arena)
{
	space_action(arena->spaceShot, action_moveShot, NULL);
}

static int isValueInList(list_t *list, int x)
{
	int i;

	for (i = 0; i < list->count; i++) {
		if (x == (*(int *) list->list[i])) {
			return 1;
		}
	}

	return 0;
}

static void action_check(space_t *space, shot_t *shot, client_t *client)
{
	(void)space;
	if (isValueInList(client->listSeesShot, shot->id) == 0) {
		list_add(client->listSeesShot, newInt(shot->id));
		proto_send_shot_server(PROTO_SEND_ONE, client, shot);
	}
}

void shot_check_is_tux_screen(arena_t *arena)
{
	int screen_x, screen_y;
	tux_t *thisTux;
	client_t *thisClient;
	list_t *listClient;
	int i;

	if (net_multiplayer_get_game_type() != NET_GAME_TYPE_SERVER) {
		return;
	}

	listClient = server_get_list_clients();

	for (i = 0; i < listClient->count; i++) {
		thisClient = (client_t *) listClient->list[i];
		thisTux = (tux_t *) thisClient->tux;
		int del = 0;

		if (thisTux == NULL || thisTux->control != TUX_CONTROL_NET) {
			continue;
		}

		arena_get_center_screen(&screen_x, &screen_y, thisTux->x, thisTux->y,
					WINDOW_SIZE_X, WINDOW_SIZE_Y);

		space_action_from_location(arena->spaceShot, action_check,
					   thisClient, screen_x, screen_y,
					   WINDOW_SIZE_X, WINDOW_SIZE_Y);

		while (thisClient->listSeesShot->count > 100) {
			list_del_item(thisClient->listSeesShot, 0, free);
			del++;
		}

		/*if( del > 0 )printf("del = %d\n", del);*/
	}
}

void shot_destroy(shot_t *p)
{
	assert(p != NULL);

	id_del(p->id);

	free(p);
}

void shot_quit()
{
	isShotInit = FALSE;
}
