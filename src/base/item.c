
#include <stdlib.h>
#include <assert.h>

#include "main.h"
#include "arena.h"
#include "tux.h"
#include "item.h"
#include "shot.h"
#include "proto.h"
#include "idManager.h"
#include "net_multiplayer.h"
#include "serverSendMsg.h"

#ifndef PUBLIC_SERVER

#    include "interface.h"
#    include "image.h"
#    include "radar.h"
#    include "layer.h"

#    include "world.h"
#    include "setting.h"

#    ifndef NO_SOUND
#        include "sound.h"
#    endif

#endif

#ifdef PUBLIC_SERVER
#    include "publicServer.h"
#endif

#ifndef PUBLIC_SERVER
static image_t *g_item[ITEM_COUNT];
#endif

static bool_t isItemInit = FALSE;

bool_t item_is_inicialized()
{
	return isItemInit;
}

void item_init()
{
#ifndef PUBLIC_SERVER
	g_item[GUN_DUAL_SIMPLE] = image_add("item.dual.png", IMAGE_ALPHA, "item_dual_simple", IMAGE_GROUP_BASE);
	g_item[GUN_TOMMY] = image_add("item.tommy.png", IMAGE_ALPHA, "item_tommy_simple", IMAGE_GROUP_BASE);
	g_item[GUN_SCATTER] = image_add("item.scatter.png", IMAGE_ALPHA, "item_scatter_simple", IMAGE_GROUP_BASE);
	g_item[GUN_LASSER] = image_add("item.lasser.png", IMAGE_ALPHA, "item_lasser_simple", IMAGE_GROUP_BASE);
	g_item[GUN_MINE] = image_add("item.mine.png", IMAGE_ALPHA, "item_mines_simple", IMAGE_GROUP_BASE);
	g_item[GUN_BOMBBALL] = image_add("item.bombball.png", IMAGE_ALPHA, "item_bombball_simple", IMAGE_GROUP_BASE);
	g_item[ITEM_MINE] = image_add("mine.png", IMAGE_ALPHA, "item_mine_simple", IMAGE_GROUP_BASE);
	g_item[ITEM_EXPLOSION] = image_add("explosion.png", IMAGE_ALPHA, "item_explosion_simple", IMAGE_GROUP_BASE);
	g_item[ITEM_BIG_EXPLOSION] = image_add("big-explosion.png", IMAGE_ALPHA, "item_big-explosion_simple", IMAGE_GROUP_BASE);
	g_item[BONUS_SPEED] = image_add("item.bonus.speed.png", IMAGE_ALPHA, "item_bonus_speed", IMAGE_GROUP_BASE);
	g_item[BONUS_SHOT] = image_add("item.bonus.shot.png", IMAGE_ALPHA, "item_shot_speed", IMAGE_GROUP_BASE);
	g_item[BONUS_TELEPORT] = image_add("item.bonus.teleport.png", IMAGE_ALPHA,  "item_teleport_speed", IMAGE_GROUP_BASE);
	g_item[BONUS_GHOST] = image_add("item.bonus.ghost.png", IMAGE_ALPHA, "item_ghost_speed",  IMAGE_GROUP_BASE);
	g_item[BONUS_4X] = image_add("item.bonus.4x.png", IMAGE_ALPHA, "item_4x_speed", IMAGE_GROUP_BASE);
	g_item[BONUS_HIDDEN] = image_add("item.bonus.hidden.png", IMAGE_ALPHA, "item_hidden_speed", IMAGE_GROUP_BASE);
#endif
	isItemInit = TRUE;
}

item_t *item_new(int x, int y, int type, int author_id)
{
	item_t *new;

	new = malloc(sizeof(item_t));
	assert(new != NULL);
	new->type = type;
	new->id = id_get_new();
	new->x = x;
	new->y = y;
	new->frame = 0;
	new->count = 0;
#ifndef PUBLIC_SERVER
	new->img = g_item[type];
#endif
	new->author_id = author_id;

	switch (type) {
		case GUN_DUAL_SIMPLE:
		case GUN_SCATTER:
		case GUN_TOMMY:
		case GUN_LASSER:
		case GUN_MINE:
		case GUN_BOMBBALL:
			new->w = ITEM_GUN_WIDTH;
			new->h = ITEM_GUN_HEIGHT;
			break;
		case ITEM_MINE:
			new->w = ITEM_MINE_WIDTH;
			new->h = ITEM_MINE_HEIGHT;
			break;
		case ITEM_EXPLOSION:
	#ifndef NO_SOUND
			sound_play("explozion", SOUND_GROUP_BASE);
	#endif
			new->w = ITEM_EXPLOSION_WIDTH;
			new->h = ITEM_EXPLOSION_HEIGHT;
			break;
		case ITEM_BIG_EXPLOSION:
	#ifndef NO_SOUND
			sound_play("explozion", SOUND_GROUP_BASE);
	#endif
			new->w = ITEM_BIG_EXPLOSION_WIDTH;
			new->h = ITEM_BIG_EXPLOSION_HEIGHT;
			break;
		case BONUS_SPEED:
		case BONUS_SHOT:
		case BONUS_TELEPORT:
		case BONUS_GHOST:
		case BONUS_4X:
		case BONUS_HIDDEN:
			new->w = ITEM_BONUS_WIDTH;
			new->h = ITEM_BONUS_HEIGHT;
			break;
	}

	return new;
}

void item_get_status(void *p, int *id, int *x, int *y, int *w, int *h)
{
	item_t *item;

	item = p;
	*id = item->id;
	*x = item->x;
	*y = item->y;
	*w = item->w;
	*h = item->h;
}

void item_set_status(void *p, int x, int y, int w, int h)
{
	item_t *item;

	item = p;
	item->x = x;
	item->y = y;
	item->w = w;
	item->h = h;
}

void item_replace_id(item_t * item, int id)
{
	id_replace(item->id, id);
	item->id = id;
}

#ifdef PUBLIC_SERVER
static void action_countitem(space_t * space, item_t * item, int *count)
{
	if ((item->type >= GUN_DUAL_SIMPLE && item->type <= GUN_BOMBBALL) ||
	    (item->type >= BONUS_SPEED && item->type <= BONUS_HIDDEN) ) {
		(*count)++;
	}
}

static int getCountWeaponOrBonusItem(space_t * spaceItem)
{
	int count = 0;

	space_action(spaceItem, action_countitem, &count);

	return count;
}
#endif

void item_add_new_item(space_t * spaceItem, int author_id)
{
	arena_t *arena;
	item_t *item;
	int new_x, new_y;
	int type;

#ifdef PUBLIC_SERVER
	if (getCountWeaponOrBonusItem(spaceItem) >= atoi(publicServer_get_setting("MAX_ITEM", "--max-item", "100"))) {
		return;
	}
#endif

#ifndef PUBLIC_SERVER
	if (setting_is_any_item() == FALSE || netMultiplayer_get_game_type() == NET_GAME_TYPE_CLIENT) {
		return;
	}
#endif

	arena = arena_get_current();
	arena_find_free_space(arena_get_current(), &new_x, &new_y, ITEM_GUN_WIDTH, ITEM_GUN_HEIGHT);

	type = GUN_DUAL_SIMPLE;

#ifndef PUBLIC_SERVER
	do {
#endif
		switch (random() % 12) {
			case 0:
				type = GUN_DUAL_SIMPLE;
				break;
			case 1:
				type = GUN_SCATTER;
				break;
			case 2:
				type = GUN_TOMMY;
				break;
			case 3:
				type = GUN_MINE;
				break;
			case 4:
				type = GUN_LASSER;
				break;
			case 5:
				type = GUN_BOMBBALL;
				break;
			case 6:
				type = BONUS_SPEED;
				break;
			case 7:
				type = BONUS_SHOT;
				break;
			case 8:
				type = BONUS_TELEPORT;
				break;
			case 9:
				type = BONUS_GHOST;
				break;
			case 10:
				type = BONUS_4X;
				break;
			case 11:
				type = BONUS_HIDDEN;
				break;
		}
#ifndef PUBLIC_SERVER
	} while (setting_is_item(type) == FALSE ||
		(netMultiplayer_get_game_type() == NET_GAME_TYPE_NONE && type == BONUS_HIDDEN));
#endif
	item = item_new(new_x, new_y, type, author_id);
	space_add(spaceItem, item);

	if (netMultiplayer_get_game_type() == NET_GAME_TYPE_SERVER)
		proto_send_additem_server(PROTO_SEND_ALL, NULL, item);

#ifndef PUBLIC_SERVER
	if (netMultiplayer_get_game_type() != NET_GAME_TYPE_CLIENT)
		radar_add(item->id, new_x, new_y, RADAR_TYPE_ITEM);
#endif
}

#ifndef PUBLIC_SERVER
void item_draw(item_t * p)
{
	assert(p != NULL);
	addLayer(p->img, p->x, p->y, p->frame * p->w, 0, p->w, p->h, TUX_LAYER);
}

void item_draw_list(list_t * listItem)
{
	item_t *thisItem;
	int i;

	assert(listItem != NULL);

	for (i = 0; i < listItem->count; i++) {
		thisItem = (item_t *) listItem->list[i];
		assert(thisItem != NULL);
		item_draw(thisItem);
	}
}

#endif

static void action_itemevent(space_t * space, item_t * item, void *p)
{
	my_time_t currentTime;

	currentTime = timer_get_current_time();
	item->count++;

	if (item->count == ITEM_MAX_COUNT) {
		item->count = 0;
		item->frame++;
	}

	switch (item->type) {
		case GUN_TOMMY:
		case GUN_DUAL_SIMPLE:
		case GUN_SCATTER:
		case GUN_LASSER:
		case GUN_MINE:
		case GUN_BOMBBALL:
			if (item->frame == ITEM_GUN_MAX_FRAME)
				item->frame = 0;
			break;
		case ITEM_MINE:
			if (item->frame == ITEM_MINE_MAX_FRAME)
				item->frame = 0;
			break;
		case ITEM_EXPLOSION:
		case ITEM_BIG_EXPLOSION:
			if (item->frame == ITEM_EXPLOSION_MAX_FRAME)
				space_del_with_item(space, item, item_destroy);
			break;
		case BONUS_SPEED:
		case BONUS_SHOT:
		case BONUS_TELEPORT:
		case BONUS_GHOST:
		case BONUS_4X:
		case BONUS_HIDDEN:
			if (item->frame == ITEM_GUN_MAX_FRAME)
				item->frame = 0;
			break;
	}
}

void item_event(space_t * spaceItem)
{
	space_action(spaceItem, action_itemevent, NULL);
}

static void mineExplosion(space_t * spaceItem, item_t * item)
{
	if (netMultiplayer_get_game_type() != NET_GAME_TYPE_CLIENT) {
		int x, y;
		item_t *item_explosion;

		x = (item->x + item->w / 2) - ITEM_BIG_EXPLOSION_WIDTH / 2;
		y = (item->y + item->h / 2) - ITEM_BIG_EXPLOSION_HEIGHT / 2;
		item_explosion = item_new(x, y, ITEM_BIG_EXPLOSION, item->author_id);

		if (netMultiplayer_get_game_type() == NET_GAME_TYPE_SERVER)
			proto_send_additem_server(PROTO_SEND_ALL, NULL, item_explosion);

		space_add(spaceItem, item_explosion);
	}

	if (netMultiplayer_get_game_type() == NET_GAME_TYPE_SERVER)
		proto_send_del_server(PROTO_SEND_ALL, NULL, item->id);

	if (netMultiplayer_get_game_type() != NET_GAME_TYPE_CLIENT)
		space_del_with_item(spaceItem, item, item_destroy);
}

static void action_item(space_t * space, item_t * item, int *isDel)
{
	arena_t *arena;

	arena = arena_get_current();

	switch (item->type) {
		case ITEM_MINE:
			if (netMultiplayer_get_game_type() != NET_GAME_TYPE_CLIENT)
				mineExplosion(space, item);
			break;
		case ITEM_EXPLOSION:
		case ITEM_BIG_EXPLOSION:
			*isDel = 1;
			break;
	}
}

static void action_shot(space_t * space, shot_t * shot, space_t * spaceItem)
{
	int isDel = 0;

	space_action_from_location(spaceItem, action_item, &isDel,
				shot->x, shot->y, shot->w, shot->h);

	if (isDel) {
		if (netMultiplayer_get_game_type() == NET_GAME_TYPE_SERVER)
			proto_send_del_server(PROTO_SEND_ALL, NULL, shot->id);

		space_del_with_item(space, shot, shot_destroy);
	}
}

void item_event_conglicts_shot_with_item(arena_t * arena)
{
	if (netMultiplayer_get_game_type() == NET_GAME_TYPE_CLIENT)
		return;

	space_action(arena->spaceShot, action_shot, arena->spaceItem);
}

static void tux_event_tux_is_deadWithItem(tux_t * tux, item_t * item)
{
	if (tux->bonus == BONUS_GHOST)
		return;

	if (tux->bonus == BONUS_TELEPORT) {
		tux_teleport(tux);
		return;
	}

	if (item->author_id == tux->id) {
		tux->score--;

		if (netMultiplayer_get_game_type() == NET_GAME_TYPE_SERVER)
			proto_send_newtux_server(PROTO_SEND_ALL, NULL, tux);
	}

	if (item->author_id != tux->id) {
		tux_t *author;

		author = space_get_object_id(arena_get_current()->spaceTux, item->author_id);

		if (author != NULL) {
			author->score++;

			if (netMultiplayer_get_game_type() == NET_GAME_TYPE_SERVER)
				proto_send_newtux_server(PROTO_SEND_ALL, NULL, author);
		}
	}

	word_inc_round();
	tux_event_tux_is_dead(tux);
}

static void tuxGiveBonus(tux_t * tux, item_t * item)
{
#ifndef NO_SOUND
	sound_play("item_bonus", SOUND_GROUP_BASE);
#endif
	tux->bonus = item->type;
	tux->bonus_time = TUX_MAX_BONUS;

	if (netMultiplayer_get_game_type() == NET_GAME_TYPE_SERVER)
		proto_send_newtux_server(PROTO_SEND_ALL, NULL, tux);

#ifndef PUBLIC_SERVER
	if (netMultiplayer_get_game_type() != NET_GAME_TYPE_CLIENT)
		radar_del(item->id);
#endif
}

static void tuxGiveGun(tux_t * tux, item_t * item)
{
#ifndef NO_SOUND
	sound_play("item_gun", SOUND_GROUP_BASE);
#endif
	tux->pickup_time = 0;
	tux->shot[item->type] += GUN_MAX_SHOT;
	tux->gun = item->type;

	if (netMultiplayer_get_game_type() == NET_GAME_TYPE_SERVER)
		proto_send_newtux_server(PROTO_SEND_ALL, NULL, tux);

#ifndef PUBLIC_SERVER
	if (netMultiplayer_get_game_type() != NET_GAME_TYPE_CLIENT)
		radar_del(item->id);
#endif
}

static void action_giveitem(space_t * space, item_t * item, tux_t * tux)
{
	switch (item->type) {
		case GUN_TOMMY:
		case GUN_DUAL_SIMPLE:
		case GUN_SCATTER:
		case GUN_LASSER:
		case GUN_MINE:
		case GUN_BOMBBALL:
			tuxGiveGun(tux, item);
	
			if (netMultiplayer_get_game_type() == NET_GAME_TYPE_SERVER)
				proto_send_del_server(PROTO_SEND_ALL, NULL, item->id);
	
			space_del_with_item(space, item, item_destroy);
	#ifdef PUBLIC_SERVER
			item_add_new_item(space, ID_UNKNOWN);
	#endif
			break;
	
		case ITEM_MINE:
			if (tux->bonus != BONUS_GHOST)
				mineExplosion(space, item);
			break;
		case ITEM_EXPLOSION:
		case ITEM_BIG_EXPLOSION:
			if (tux->bonus != BONUS_GHOST)
				tux_event_tux_is_deadWithItem(tux, item);
			break;
		case BONUS_SPEED:
		case BONUS_SHOT:
		case BONUS_TELEPORT:
		case BONUS_GHOST:
		case BONUS_4X:
		case BONUS_HIDDEN:
			tuxGiveBonus(tux, item);
	
			if (netMultiplayer_get_game_type() == NET_GAME_TYPE_SERVER)
				proto_send_del_server(PROTO_SEND_ALL, NULL, item->id);
	
			space_del_with_item(space, item, item_destroy);
	#ifdef PUBLIC_SERVER
			item_add_new_item(space, ID_UNKNOWN);
	#endif
			break;
	}
}

void item_tux_give_list_item(tux_t * tux, space_t * spaceItem)
{
	int x, y, w, h;

	assert(spaceItem != NULL);
	assert(tux != NULL);

	if (netMultiplayer_get_game_type() == NET_GAME_TYPE_CLIENT)
		return;

	if (tux->status == TUX_STATUS_DEAD)
		return;

	tux_get_proportion(tux, &x, &y, &w, &h);
	space_action_from_location(spaceItem, action_giveitem, tux, x, y, w, h);
}

void item_destroy(item_t * p)
{
	assert(p != NULL);
	id_del(p->id);
	free(p);
}

void item_quiy()
{
	isItemInit = FALSE;
}
