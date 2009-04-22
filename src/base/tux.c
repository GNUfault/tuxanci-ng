
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "main.h"
#include "tux.h"
#include "arena.h"
#include "shot.h"
#include "gun.h"
#include "item.h"
#include "arena.h"
#include "myTimer.h"
#include "net_multiplayer.h"
#include "proto.h"
#include "modules.h"
#include "idManager.h"
#include "serverSendMsg.h"

#ifndef PUBLIC_SERVER
#    include "font.h"
#    include "image.h"
#    include "layer.h"
#    include "radar.h"

#    ifndef NO_SOUND
#        include "sound.h"
#    endif

#    include "world.h"
#endif

#ifdef PUBLIC_SERVER
#    include "publicServer.h"
#endif

#ifndef PUBLIC_SERVER

static image_t *g_tux_up;
static image_t *g_tux_right;
static image_t *g_tux_left;
static image_t *g_tux_down;
static image_t *g_cross;

#endif

static bool_t isTuxInit = FALSE;

bool_t tux_is_inicialized()
{
	return isTuxInit;
}

void tux_init()
{
#ifndef PUBLIC_SERVER
	assert(image_is_inicialized() == TRUE);
#endif

#ifndef PUBLIC_SERVER
	g_tux_up = image_add("tux8.png", IMAGE_ALPHA, "tux8", IMAGE_GROUP_BASE);
	g_tux_right = image_add("tux6.png", IMAGE_ALPHA, "tux6", IMAGE_GROUP_BASE);
	g_tux_left = image_add("tux4.png", IMAGE_ALPHA, "tux4", IMAGE_GROUP_BASE);
	g_tux_down = image_add("tux2.png", IMAGE_ALPHA, "tux2", IMAGE_GROUP_BASE);
	g_cross = image_add("cross.png", IMAGE_ALPHA, "cross", IMAGE_GROUP_BASE);
#endif
	isTuxInit = TRUE;
}

int getRandomGun()
{
	return random() % (GUN_BOMBBALL+1);
}

tux_t *tux_new()
{
	int x, y;
	tux_t *new;

	new = malloc(sizeof(tux_t));
	assert(new != NULL);

	memset(new, 0, sizeof(tux_t));

	new->id = id_get_new();
	new->status = TUX_STATUS_ALIVE;
	new->control = TUX_CONTROL_NONE;

	arena_find_free_space(arena_get_current(), &x, &y, TUX_WIDTH, TUX_HEIGHT);
	tux_set_proportion(new, x, y);

	new->position = TUX_DOWN;
	new->gun = getRandomGun();
	new->shot[new->gun] = GUN_MAX_SHOT;

	sprintf(new->name, "no_name_id_%d", new->id);
	new->score = 0;
	new->frame = 0;

	new->bonus = BONUS_NONE;
	new->bonus_time = 0;

	new->isCanShot = TRUE;
	new->isCanSwitchGun = TRUE;

	new->client = NULL;

	return new;
}

void tux_set_name(tux_t * tux, char *name)
{
	strcpy(tux->name, name);
	//tux->g_name = getFontImage(name, COLOR_WHITE);
}

bool_t tux_is_any_gun(tux_t * tux)
{
	int i;

	for (i = 0; i < GUN_COUNT; i++) {
		if (tux->shot[i] > 0) {
			return TRUE;
		}
	}

	return FALSE;
}

void tux_get_course(int n, int *x, int *y)
{
	assert(x != NULL);
	assert(y != NULL);

	switch (n) {
		case TUX_UP:
			*x = 0;
			*y = -1;
			break;
	
		case TUX_LEFT:
			*x = -1;
			*y = 0;
			break;
	
		case TUX_RIGHT:
			*x = +1;
			*y = 0;
			break;
	
		case TUX_DOWN:
			*x = 0;
			*y = +1;
			break;
	
		default:
			assert(!_("Type variable has a really wierd value!"));
			break;
	}
}

#ifndef PUBLIC_SERVER

void tux_draw(tux_t * tux)
{
	image_t *g_image = NULL;

	assert(tux != NULL);

	if (tux->bonus == BONUS_HIDDEN &&
	    net_multiplayer_get_game_type() != NET_GAME_TYPE_NONE &&
	    tux->control == TUX_CONTROL_NET) {
		return;
	}

	switch (tux->position) {
		case TUX_UP:
			g_image = g_tux_up;
			break;
	
		case TUX_LEFT:
			g_image = g_tux_left;
			break;
	
		case TUX_RIGHT:
			g_image = g_tux_right;
			break;
	
		case TUX_DOWN:
			g_image = g_tux_down;
			break;
	
		default:
			assert(!_("p->control has a really wierd value!"));
			break;
	}

	if (tux->status == TUX_STATUS_DEAD) {
		g_image = g_cross;

		addLayer(g_cross, tux->x - g_cross->w / 2,
				 (tux->y + g_cross->h / 2) - g_cross->h,
				 0, 0, g_cross->w, g_cross->h, TUX_LAYER);

		return;
	}
/*
	if( tux->g_name != NULL )
	{
		addLayer(tux->g_name,
			tux->x - TUX_IMG_WIDTH / 2,
			( tux->y + TUX_HEIGHT / 2 ) - TUX_IMG_HEIGHT,
			0, 0,
			tux->g_name->w, tux->g_name->h, TUX_LAYER);
	}
*/
	addLayer(g_image, tux->x - TUX_IMG_WIDTH / 2,
			 (tux->y + TUX_HEIGHT / 2) - TUX_IMG_HEIGHT,
			 (tux->frame / TUX_KEY) * TUX_IMG_WIDTH, 0,
			 TUX_IMG_WIDTH, TUX_IMG_HEIGHT, TUX_LAYER);
}

/*
void drawListTux(list_t *listTux)
{
	tux_t *thisTux;
	int i;

	assert( listTux != NULL );

	for( i = 0 ; i < listTux->count ; i++ )
	{
		thisTux  = (tux_t *)listTux->list[i];
		assert( thisTux != NULL );
		tux_draw(thisTux);
	}
}
*/
#endif

void tux_replace_id(tux_t * tux, int id)
{
	id_replace(tux->id, id);
	tux->id = id;
}

static void timer_spawnTux(void *p)
{
	tux_t *tux;
	arena_t *arena;
	int x, y;
	int id;

	id = *((int *) p);
	free(p);

	arena = arena_get_current();
	tux = space_get_object_id(arena->spaceTux, id);

	if (tux == NULL)
		return;

	tux->status = TUX_STATUS_ALIVE;
	arena_find_free_space(arena_get_current(), &x, &y, TUX_WIDTH, TUX_HEIGHT);
	space_move_object(arena_get_current()->spaceTux, tux, x, y);
	tux->gun = GUN_SIMPLE;

	item_add_new_item(arena->spaceItem, ID_UNKNOWN);

	if (net_multiplayer_get_game_type() == NET_GAME_TYPE_SERVER) {
		proto_send_newtux_server(PROTO_SEND_ALL, NULL, tux);
	}
}

static void timer_tuxCanShot(void *p)
{
	tux_t *tux;
	int id;

	id = *((int *) p);
	free(p);

	tux = space_get_object_id(arena_get_current()->spaceTux, id);

	if (tux == NULL)
		return;

	tux->isCanShot = TRUE;
}

static void timer_tuxCanSwitchGun(void *p)
{
	tux_t *tux;
	int id;

	id = *((int *) p);
	free(p);

	tux = space_get_object_id(arena_get_current()->spaceTux, id);

	if (tux == NULL)
		return;

	tux->isCanSwitchGun = TRUE;
}

void tux_event_tux_is_dead(tux_t * tux)
{
	if (tux->status == TUX_STATUS_DEAD) {
		return;
	}

#ifndef NO_SOUND
	sound_play("dead", SOUND_GROUP_BASE);
#endif

	tux->status = TUX_STATUS_DEAD;
	memset(tux->shot, 0, sizeof(int) * GUN_COUNT);
	tux->gun = GUN_SIMPLE;
	tux->shot[tux->gun] = GUN_MAX_SHOT;

	tux->bonus = BONUS_NONE;
	tux->bonus_time = 0;
	tux->pickup_time = 0;

	tux->isCanShot = TRUE;
	tux->isCanSwitchGun = TRUE;

	if (net_multiplayer_get_game_type() != NET_GAME_TYPE_CLIENT) {
		timer_add_task(arena_get_current()->listTimer, TIMER_ONE,
			       timer_spawnTux, newInt(tux->id), TUX_TIME_SPAWN);
	}

	if (net_multiplayer_get_game_type() == NET_GAME_TYPE_SERVER) {
		proto_send_kill_server(PROTO_SEND_ALL, NULL, tux);
	}
}

static void tux_event_tux_is_deadWIthShot(tux_t * tux, shot_t * shot)
{
	if (shot->author_id == tux->id) {
		tux->score--;

		if (net_multiplayer_get_game_type() == NET_GAME_TYPE_SERVER) {
			proto_send_newtux_server(PROTO_SEND_ALL, NULL, tux);
		}
	}

	if (shot->author_id != tux->id) {
		tux_t *author;

		author = space_get_object_id(arena_get_current()->spaceTux, shot->author_id);

		if (author != NULL) {
			author->score++;

			if (net_multiplayer_get_game_type() == NET_GAME_TYPE_SERVER) {
				proto_send_newtux_server(PROTO_SEND_ALL, NULL, author);
			}
		}

	}

	world_inc_round();
	tux_event_tux_is_dead(tux);
}

void tux_teleport(tux_t * tux)
{
	int x, y;

#ifndef NO_SOUND
	sound_play("teleport", SOUND_GROUP_BASE);
#endif

	if (net_multiplayer_get_game_type() != NET_GAME_TYPE_CLIENT) {
		arena_find_free_space(arena_get_current(), &x, &y, TUX_WIDTH, TUX_HEIGHT);
		space_move_object(arena_get_current()->spaceTux, tux, x, y);
		//tux_set_proportion(tux, x, y);
	}

	if (net_multiplayer_get_game_type() == NET_GAME_TYPE_SERVER) {
		proto_send_newtux_server(PROTO_SEND_ALL, NULL, tux);
	}
}

static void bombBallExplosion(shot_t * shot)
{
	item_t *item;
	int x, y;

	x = (shot->x + shot->w / 2) - ITEM_BIG_EXPLOSION_WIDTH / 2;
	y = (shot->y + shot->h / 2) - ITEM_BIG_EXPLOSION_HEIGHT / 2;

	item = item_new(x, y, ITEM_BIG_EXPLOSION, shot->author_id);

	space_add(arena_get_current()->spaceItem, item);

	if (net_multiplayer_get_game_type() == NET_GAME_TYPE_SERVER) {
		proto_send_del_server(PROTO_SEND_ALL, NULL, shot->id);
		proto_send_additem_server(PROTO_SEND_ALL, NULL, item);
	}

	space_del_with_item(arena_get_current()->spaceShot, shot, shot_destroy);
}

static void action_tux(space_t * space, tux_t * tux, shot_t * shot)
{
	arena_t *arena;

	UNUSET(space);
	
	arena = arena_get_current();

	if (tux->status == TUX_STATUS_ALIVE) {
		if (shot->author_id == tux->id && shot->isCanKillAuthor == FALSE) {
			return;
		}

		if (tux->bonus == BONUS_TELEPORT) {
			tux_teleport(tux);
			return;
		}

		if (shot->gun == GUN_BOMBBALL) {
			if (net_multiplayer_get_game_type() != NET_GAME_TYPE_CLIENT) {
				bombBallExplosion(shot);
			}

			return;
		}

		if (net_multiplayer_get_game_type() != NET_GAME_TYPE_CLIENT) {
			tux_event_tux_is_deadWIthShot(tux, shot);
		}
	}

	if (net_multiplayer_get_game_type() == NET_GAME_TYPE_SERVER) {
		proto_send_del_server(PROTO_SEND_ALL, NULL, shot->id);
	}

	space_del_with_item(arena->spaceShot, shot, shot_destroy);
}

static void action_shot(space_t * space, shot_t * shot, space_t * spaceTux)
{
	UNUSET(space);

	space_action_from_location(spaceTux, action_tux, shot, shot->x, shot->y, shot->w, shot->h);
}

void tux_conflict_woth_shot(arena_t * arena)
{
	if (net_multiplayer_get_game_type() == NET_GAME_TYPE_CLIENT) { // na skusku
		return;
	}

	space_action(arena->spaceShot, action_shot, arena->spaceTux);
}

void moveTux(tux_t * tux, int n)
{
	int px = 0, py = 0; // no warninng
	int zal_x, zal_y;
	int new_x, new_y;
	int w, h;
	arena_t *arena;

	assert(tux != NULL);

	arena = arena_get_current();
	//interval();

	if (tux->position != n) {
		tux->position = n;
		return;
	}

	if ((n == TUX_LEFT && tux->x - TUX_STEP <= 0) ||
	    (n == TUX_RIGHT && tux->x + TUX_STEP >= arena->w) ||
	    (n == TUX_UP && tux->y - TUX_STEP <= 0) ||
	    (n == TUX_DOWN && tux->y + TUX_STEP >= arena->h)) {
		return;
	}

	tux_get_proportion(tux, &zal_x, &zal_y, &w, &h);

	tux_get_course(tux->position, &px, &py);

	new_x = zal_x;
	new_y = zal_y;

	new_x += px * TUX_STEP;
	new_y += py * TUX_STEP;

	if (tux->bonus == BONUS_SPEED) {
		new_x += px * TUX_STEP;
		new_y += py * TUX_STEP;
	}

	if (tux->bonus != BONUS_GHOST &&
	   ( space_is_conflict_with_object_but(arena->spaceTux, new_x, new_y, w, h, tux) ||
	     module_is_conflict(new_x, new_y, w, h))) {
		tux_set_proportion(tux, zal_x, zal_y);
		return;
	}

	tux_set_proportion(tux, zal_x, zal_y);
	space_move_object(arena->spaceTux, tux, new_x, new_y);
	item_tux_give_list_item(tux, arena_get_current()->spaceItem);
	tux->frame++;

	if (tux->frame == TUX_KEY * TUX_MAX_ANIMATION_FRAME)
		tux->frame = 0;
}

void switchTuxGun(tux_t * tux)
{
	int i;

	if (tux->control != TUX_CONTROL_NET && tux->isCanSwitchGun == FALSE) {
		return;
	}

	for (i = tux->gun + 1; i < GUN_COUNT; i++) {
		if (tux->shot[i] > 0) {
			tux->gun = i;
			tux->isCanSwitchGun = FALSE;
			timer_add_task(arena_get_current()->listTimer, TIMER_ONE,
					timer_tuxCanSwitchGun, newInt(tux->id),
					TUX_TIME_CAN_SWITCH_GUN);
#ifndef NO_SOUND
			sound_play("switch_gun", SOUND_GROUP_BASE);
#endif
			return;
		}
	}

	for (i = 0; i < GUN_COUNT; i++) {
		if (tux->shot[i] > 0) {
			tux->gun = i;
			tux->isCanSwitchGun = FALSE;

			timer_add_task(arena_get_current()->listTimer, TIMER_ONE,
				       timer_tuxCanSwitchGun, newInt(tux->id),
				       TUX_TIME_CAN_SWITCH_GUN);

#ifndef NO_SOUND
			sound_play("switch_gun", SOUND_GROUP_BASE);
#endif
			return;
		}
	}
}

void shotTux(tux_t * tux)
{
	assert(tux != NULL);

	if (tux->isCanShot == FALSE || tux->shot[tux->gun] == 0) {
		return;
	}

#ifndef NO_SOUND
	sound_play("switch_gun", SOUND_GROUP_BASE);
#endif

	gun_shot(tux);

	tux->isCanShot = FALSE;

	timer_add_task(arena_get_current()->listTimer, TIMER_ONE, timer_tuxCanShot,
		       newInt(tux->id), TUX_TIME_CAN_SHOT);

	if (tux->shot[tux->gun] == 0) {
		switchTuxGun(tux);
	}
}

void tux_action(tux_t * tux, int action)
{
	if (tux->status == TUX_STATUS_DEAD) {
		return;
	}

	switch (action) {
		case TUX_UP:
		case TUX_LEFT:
		case TUX_RIGHT:
		case TUX_DOWN:
			moveTux(tux, action);
			break;
	
		case TUX_SHOT:
			shotTux(tux);
			break;
	
		case TUX_SWITCH_GUN:
			switchTuxGun(tux);
			break;
	}
}

static void pickUpGun(tux_t * tux)
{
	if (tux_is_any_gun(tux) == FALSE) {
		tux->gun = GUN_SIMPLE;

		if (tux->pickup_time < TUX_MAX_PICKUP) {
			tux->pickup_time++;
		}

		if (tux->pickup_time == TUX_MAX_PICKUP && net_multiplayer_get_game_type() != NET_GAME_TYPE_CLIENT) {
#ifndef NO_SOUND
			sound_play("switch_gun", SOUND_GROUP_BASE);
#endif

			tux->gun = GUN_SIMPLE;
			tux->shot[tux->gun] = GUN_MAX_SHOT;
			tux->pickup_time = 0;

			if (net_multiplayer_get_game_type() == NET_GAME_TYPE_SERVER) {
				proto_send_newtux_server(PROTO_SEND_ALL, NULL, tux);
			}
		}
	}
}

static void eventBonus(tux_t * tux)
{
	if (tux->bonus != BONUS_NONE) {
		if (tux->bonus_time > 0) {
			tux->bonus_time--;
		}

		if (tux->bonus_time == 0 && net_multiplayer_get_game_type() != NET_GAME_TYPE_CLIENT) {
			if (tux->bonus == BONUS_GHOST) {
				int x, y, w, h;
				tux_get_proportion(tux, &x, &y, &w, &h);

				tux->bonus = BONUS_NONE;

				if ( /*isConflictTuxWithListTux(tux, arena_get_current()->listTux) || */
				     module_is_conflict(x, y, w, h)) {
					tux->bonus = BONUS_GHOST;
					return;
				}
			}

			tux->bonus = BONUS_NONE;

			if (net_multiplayer_get_game_type() == NET_GAME_TYPE_SERVER) {
				proto_send_newtux_server(PROTO_SEND_ALL, NULL, tux);
			}
		}
	}
}

void tux_event(tux_t * tux)
{
	arena_t *arena;

	arena = arena_get_current();

#ifndef PUBLIC_SERVER
	world_tux_control(tux);
#endif
	pickUpGun(tux);
	eventBonus(tux);
	item_tux_give_list_item(tux, arena->spaceItem);
}

/*
void eventListTux(list_t *listTux)
{
	tux_t *thisTux;
	int i;

	assert( listTux != NULL );

	for( i = 0 ; i < listTux->count ; i++ )
	{
		thisTux  = (tux_t *)listTux->list[i];
		assert( thisTux != NULL );
		tux_event(thisTux);
	}
}
*/
void tux_get_proportion(tux_t * tux, int *x, int *y, int *w, int *h)
{
	assert(tux != NULL);

	if (x != NULL)
		*x = tux->x - TUX_WIDTH / 2;

	if (y != NULL)
		*y = tux->y - TUX_HEIGHT / 2;

	if (w != NULL)
		*w = TUX_WIDTH;

	if (h != NULL)
		*h = TUX_HEIGHT;
}

void tux_set_proportion(tux_t * tux, int x, int y)
{
	assert(tux != NULL);

	tux->x = x + TUX_WIDTH / 2;
	tux->y = y + TUX_WIDTH / 2;
}

void tux_get_status(void *p, int *id, int *x, int *y, int *w, int *h)
{
	tux_t *tux;

	tux = p;
	*id = tux->id;
	tux_get_proportion(tux, x, y, w, h);
}

void tux_set_status(void *p, int x, int y, int w, int h)
{
	tux_t *tux;

	UNUSET(w);
	UNUSET(h);

	tux = p;
	tux_set_proportion(tux, x, y);
}

static void action_checkMine(space_t * space, item_t * item, tux_t * tux)
{
	UNUSET(space);

	if (item->type == ITEM_MINE && item->author_id == tux->id) {
		item->author_id = ID_UNKNOWN;
	}
}

void tux_destroy(tux_t * tux)
{
	arena_t *arena;

	arena = arena_get_current();

	assert(tux != NULL);

	id_del(tux->id);

	space_action(arena->spaceItem, action_checkMine, tux);

	free(tux);
}

void tux_quit()
{
	isTuxInit = FALSE;
}
