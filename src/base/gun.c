
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "main.h"
#include "arena.h"
#include "tux.h"
#include "shot.h"
#include "myTimer.h"
#include "item.h"
#include "gun.h"
#include "proto.h"
#include "net_multiplayer.h"
#include "serverSendMsg.h"

#ifndef NO_SOUND
#    include "sound.h"
#endif
#ifndef PUBLIC_SERVER
#    include "interface.h"
#    include "world.h"
#endif
#ifdef PUBLIC_SERVER
#    include "publicServer.h"
#endif

static void modificiationCopuse(int courser, int right_x, int right_y, int *dest_x, int *dest_y)
{
	assert(dest_x != NULL);
	assert(dest_y != NULL);

	switch (courser) {
		case TUX_UP:
			*dest_x = right_y;
			*dest_y = -right_x;
			break;
		case TUX_RIGHT:
			*dest_x = right_x;
			*dest_y = right_y;
			break;
		case TUX_LEFT:
			*dest_x = -right_x;
			*dest_y = right_y;
			break;
		case TUX_DOWN:
			*dest_x = right_y;
			*dest_y = right_x;
			break;
		default:
			assert(!_("There is wrong value in course variable!"));
			break;
	}
}

static void addShotTrivial(tux_t * tux, int x, int y, int px, int py, int gun)
{
	int dest_x = 0, dest_y = 0;
	int dest_px = 0, dest_py = 0;
	shot_t *shot;

	modificiationCopuse(tux->position, px, py, &dest_px, &dest_py);
	modificiationCopuse(tux->position, x, y, &dest_x, &dest_y);

	if (gun == GUN_LASSER)
		switch (tux->position) {
			case TUX_UP:
				dest_y -= GUN_LASSER_HORIZONTAL;
				break;
			case TUX_RIGHT:
				break;
			case TUX_LEFT:
				dest_x -= GUN_LASSER_HORIZONTAL;
				break;
			case TUX_DOWN:
				break;
		}

	shot = shot_new(tux->x + dest_x, tux->y + dest_y, dest_px, dest_py, gun, tux->id);
	space_add(arena_get_current()->spaceShot, shot);
}

static void addShot(tux_t * tux, int x, int y, int px, int py)
{
	int gun;

	if (netMultiplayer_get_game_type() == NET_GAME_TYPE_CLIENT)
		return;

	if (tux->gun == GUN_BOMBBALL)
		gun = GUN_BOMBBALL;
	else if (tux->gun == GUN_LASSER)
		gun = GUN_LASSER;
	else
		gun = GUN_SIMPLE;

	if (tux->bonus == BONUS_4X) {
		int zal;
		int i;

		zal = tux->position;

		for (i = 0; i < 4; i++) {
			switch (i) {
			case 0:
				tux->position = TUX_UP;
				break;
			case 1:
				tux->position = TUX_RIGHT;
				break;
			case 2:
				tux->position = TUX_LEFT;
				break;
			case 3:
				tux->position = TUX_DOWN;
				break;
			}

			addShotTrivial(tux, x, y, px, py, gun);
		}

		tux->position = zal;
	} else {
		addShotTrivial(tux, x, y, px, py, gun);
	}
}

static void gun_shotSimpe(tux_t * tux)
{
	addShot(tux, 0, 0, +5, 0);
}

static void gun_shotDualSimpe(tux_t * tux)
{
	addShot(tux, 0, -10, +6, 0);
	addShot(tux, 0, +10, +6, 0);
}

static void gun_shotScatter(tux_t * tux)
{
	int i;

	for (i = -2; i <= +2; i++)
		addShot(tux, 0, 0, +8, i);
}

static void timer_addShotTimer(void *p)
{
	tux_t *tux;
	int id;
	int gun;

	id = *((int *) p);
	free(p);

	tux = space_get_object_id(arena_get_current()->spaceTux, id);

	if (tux == NULL)
		return;

	if (tux->status != TUX_STATUS_ALIVE)
		return;

	gun = tux->gun;
	tux->gun = GUN_SIMPLE;
	addShot(tux, 0, 0, +6, 0);
	tux->gun = gun;
}

static void gun_shotTommy(tux_t * tux)
{
	int i;

	for (i = 0; i < 10; i++)
		timer_add_task(arena_get_current()->listTimer, TIMER_ONE,
			       timer_addShotTimer, newInt(tux->id), i * 100);
}

static void timer_addLaserTimer(void *p)
{
	tux_t *tux;
	int id;
	int gun;

	id = *((int *) p);
	free(p);

	tux = space_get_object_id(arena_get_current()->spaceTux, id);

	if (tux == NULL)
		return;

	if (tux->status != TUX_STATUS_ALIVE)
		return;

	gun = tux->gun;
	tux->gun = GUN_LASSER;

	addShot(tux, 0, 0, +10, 0);
	addShot(tux, +40, 0, +10, 0);
	//addShot(tux, +40, 0, +10, 0);
	tux->gun = gun;
}

static void gun_shotLasser(tux_t * tux)
{
	int i;

	for (i = 0; i < 50; i++)
		timer_add_task(arena_get_current()->listTimer, TIMER_ONE,
			       timer_addLaserTimer, newInt(tux->id), i * 10);
}

static void putInGunMine(tux_t * tux)
{
	int x, y;
	arena_t *arena;

	x = tux->x;
	y = tux->y;

	switch (tux->position) {
		case TUX_UP:
			y += TUX_HEIGHT;
			break;
		case TUX_RIGHT:
			x -= TUX_WIDTH;
			break;
		case TUX_LEFT:
			x += TUX_WIDTH;
			break;
		case TUX_DOWN:
			y -= TUX_HEIGHT;
			break;
	}

	arena = arena_get_current();

	if (arena__is_free_space(arena_get_current(), x, y, ITEM_MINE_WIDTH, ITEM_MINE_HEIGHT)) {
		item_t *item;
#ifndef NO_SOUND
		sound_play("put_mine", SOUND_GROUP_BASE);
#endif
		tux->shot[tux->gun]--;

		if (netMultiplayer_get_game_type() != NET_GAME_TYPE_CLIENT) {
			item = item_new(x, y, ITEM_MINE, tux->id);

			if (netMultiplayer_get_game_type() == NET_GAME_TYPE_SERVER)
				proto_send_additem_server(PROTO_SEND_ALL, NULL, item);

			space_add(arena->spaceItem, item);
		}
	}
}

static void gun_shotBombball(tux_t * tux)
{
	addShot(tux, 0, 0, +10, 0);
}

void gun_shot(tux_t * tux)
{
	if (tux->bonus != BONUS_SHOT && tux->gun != GUN_MINE)
		tux->shot[tux->gun]--;

	switch (tux->gun) {
		case GUN_SIMPLE:
	#ifndef NO_SOUND
			sound_play("gun_revolver", SOUND_GROUP_BASE);
	#endif
			gun_shotSimpe(tux);
			break;
		case GUN_DUAL_SIMPLE:
	#ifndef NO_SOUND
			sound_play("gun_revolver", SOUND_GROUP_BASE);
	#endif
			gun_shotDualSimpe(tux);
			break;
		case GUN_SCATTER:
	#ifndef NO_SOUND
			sound_play("gun_scatter", SOUND_GROUP_BASE);
	#endif
			gun_shotScatter(tux);
			break;
		case GUN_TOMMY:
	#ifndef NO_SOUND
			sound_play("gun_tommy", SOUND_GROUP_BASE);
	#endif
			gun_shotTommy(tux);
			break;
		case GUN_LASSER:
	#ifndef NO_SOUND
			sound_play("gun_lasser", SOUND_GROUP_BASE);
	#endif
			gun_shotLasser(tux);
			break;
		case GUN_BOMBBALL:
			gun_shotBombball(tux);
			break;
		case GUN_MINE:
			putInGunMine(tux);
			break;
	}
}
