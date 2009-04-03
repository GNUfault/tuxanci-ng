
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

bool_t isTuxInicialized()
{
	return isTuxInit;
}

void initTux()
{
#ifndef PUBLIC_SERVER
	assert(isImageInicialized() == TRUE);
#endif

#ifndef PUBLIC_SERVER
	g_tux_up = addImageData("tux8.png", IMAGE_ALPHA, "tux8", IMAGE_GROUP_BASE);
	g_tux_right = addImageData("tux6.png", IMAGE_ALPHA, "tux6", IMAGE_GROUP_BASE);
	g_tux_left = addImageData("tux4.png", IMAGE_ALPHA, "tux4", IMAGE_GROUP_BASE);
	g_tux_down = addImageData("tux2.png", IMAGE_ALPHA, "tux2", IMAGE_GROUP_BASE);
	g_cross = addImageData("cross.png", IMAGE_ALPHA, "cross", IMAGE_GROUP_BASE);
#endif
	isTuxInit = TRUE;
}

int getRandomGun()
{
	return random() % (GUN_BOMBBALL+1);
}

tux_t *newTux()
{
	int x, y;
	tux_t *new;

	new = malloc(sizeof(tux_t));
	assert(new != NULL);

	memset(new, 0, sizeof(tux_t));

	new->id = getNewID();
	new->status = TUX_STATUS_ALIVE;
	new->control = TUX_CONTROL_NONE;

	findFreeSpace(getCurrentArena(), &x, &y, TUX_WIDTH, TUX_HEIGHT);
	setTuxProportion(new, x, y);

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

void tuxSetName(tux_t * tux, char *name)
{
	strcpy(tux->name, name);
	//tux->g_name = getFontImage(name, COLOR_WHITE);
}

bool_t isTuxAnyGun(tux_t * tux)
{
	int i;

	for (i = 0; i < GUN_COUNT; i++) {
		if (tux->shot[i] > 0) {
			return TRUE;
		}
	}

	return FALSE;
}

void getCourse(int n, int *x, int *y)
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

void drawTux(tux_t * tux)
{
	image_t *g_image = NULL;

	assert(tux != NULL);

	if (tux->bonus == BONUS_HIDDEN &&
	    getNetTypeGame() != NET_GAME_TYPE_NONE &&
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
		drawTux(thisTux);
	}
}
*/
#endif

void replaceTuxID(tux_t * tux, int id)
{
	replaceID(tux->id, id);
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

	arena = getCurrentArena();
	tux = getObjectFromSpaceWithID(arena->spaceTux, id);

	if (tux == NULL)
		return;

	tux->status = TUX_STATUS_ALIVE;
	findFreeSpace(getCurrentArena(), &x, &y, TUX_WIDTH, TUX_HEIGHT);
	moveObjectInSpace(getCurrentArena()->spaceTux, tux, x, y);
	tux->gun = GUN_SIMPLE;

	addNewItem(arena->spaceItem, ID_UNKNOWN);

	if (getNetTypeGame() == NET_GAME_TYPE_SERVER) {
		proto_send_newtux_server(PROTO_SEND_ALL, NULL, tux);
	}
}

static void timer_tuxCanShot(void *p)
{
	tux_t *tux;
	int id;

	id = *((int *) p);
	free(p);

	tux = getObjectFromSpaceWithID(getCurrentArena()->spaceTux, id);

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

	tux = getObjectFromSpaceWithID(getCurrentArena()->spaceTux, id);

	if (tux == NULL)
		return;

	tux->isCanSwitchGun = TRUE;
}

void eventTuxIsDead(tux_t * tux)
{
	if (tux->status == TUX_STATUS_DEAD) {
		return;
	}

#ifndef NO_SOUND
	playSound("dead", SOUND_GROUP_BASE);
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

	if (getNetTypeGame() != NET_GAME_TYPE_CLIENT) {
		addTaskToTimer(getCurrentArena()->listTimer, TIMER_ONE,
			       timer_spawnTux, newInt(tux->id), TUX_TIME_SPAWN);
	}

	if (getNetTypeGame() == NET_GAME_TYPE_SERVER) {
		proto_send_kill_server(PROTO_SEND_ALL, NULL, tux);
	}
}

static void eventTuxIsDeadWIthShot(tux_t * tux, shot_t * shot)
{
	if (shot->author_id == tux->id) {
		tux->score--;

		if (getNetTypeGame() == NET_GAME_TYPE_SERVER) {
			proto_send_newtux_server(PROTO_SEND_ALL, NULL, tux);
		}
	}

	if (shot->author_id != tux->id) {
		tux_t *author;

		author = getObjectFromSpaceWithID(getCurrentArena()->spaceTux, shot->author_id);

		if (author != NULL) {
			author->score++;

			if (getNetTypeGame() == NET_GAME_TYPE_SERVER) {
				proto_send_newtux_server(PROTO_SEND_ALL, NULL, author);
			}
		}

	}

	countRoundInc();
	eventTuxIsDead(tux);
}

void tuxTeleport(tux_t * tux)
{
	int x, y;

#ifndef NO_SOUND
	playSound("teleport", SOUND_GROUP_BASE);
#endif

	if (getNetTypeGame() != NET_GAME_TYPE_CLIENT) {
		findFreeSpace(getCurrentArena(), &x, &y, TUX_WIDTH, TUX_HEIGHT);
		moveObjectInSpace(getCurrentArena()->spaceTux, tux, x, y);
		//setTuxProportion(tux, x, y);
	}

	if (getNetTypeGame() == NET_GAME_TYPE_SERVER) {
		proto_send_newtux_server(PROTO_SEND_ALL, NULL, tux);
	}
}

static void bombBallExplosion(shot_t * shot)
{
	item_t *item;
	int x, y;

	x = (shot->x + shot->w / 2) - ITEM_BIG_EXPLOSION_WIDTH / 2;
	y = (shot->y + shot->h / 2) - ITEM_BIG_EXPLOSION_HEIGHT / 2;

	item = newItem(x, y, ITEM_BIG_EXPLOSION, shot->author_id);

	addObjectToSpace(getCurrentArena()->spaceItem, item);

	if (getNetTypeGame() == NET_GAME_TYPE_SERVER) {
		proto_send_del_server(PROTO_SEND_ALL, NULL, shot->id);
		proto_send_additem_server(PROTO_SEND_ALL, NULL, item);
	}

	delObjectFromSpaceWithObject(getCurrentArena()->spaceShot, shot, destroyShot);
}

static void action_tux(space_t * space, tux_t * tux, shot_t * shot)
{
	arena_t *arena;

	arena = getCurrentArena();

	if (tux->status == TUX_STATUS_ALIVE) {
		if (shot->author_id == tux->id && shot->isCanKillAuthor == FALSE) {
			return;
		}

		if (tux->bonus == BONUS_TELEPORT) {
			tuxTeleport(tux);
			return;
		}

		if (shot->gun == GUN_BOMBBALL) {
			if (getNetTypeGame() != NET_GAME_TYPE_CLIENT) {
				bombBallExplosion(shot);
			}

			return;
		}

		if (getNetTypeGame() != NET_GAME_TYPE_CLIENT) {
			eventTuxIsDeadWIthShot(tux, shot);
		}
	}

	if (getNetTypeGame() == NET_GAME_TYPE_SERVER) {
		proto_send_del_server(PROTO_SEND_ALL, NULL, shot->id);
	}

	delObjectFromSpaceWithObject(arena->spaceShot, shot, destroyShot);
}

static void action_shot(space_t * space, shot_t * shot, space_t * spaceTux)
{
	actionSpaceFromLocation(spaceTux, action_tux, shot, shot->x, shot->y, shot->w, shot->h);
}

void eventConflictTuxWithShot(arena_t * arena)
{
	if (getNetTypeGame() == NET_GAME_TYPE_CLIENT) { // na skusku
		return;
	}

	actionSpace(arena->spaceShot, action_shot, arena->spaceTux);
}

void moveTux(tux_t * tux, int n)
{
	int px = 0, py = 0; // no warninng
	int zal_x, zal_y;
	int new_x, new_y;
	int w, h;
	arena_t *arena;

	assert(tux != NULL);

	arena = getCurrentArena();
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

	getTuxProportion(tux, &zal_x, &zal_y, &w, &h);

	getCourse(tux->position, &px, &py);

	new_x = zal_x;
	new_y = zal_y;

	new_x += px * TUX_STEP;
	new_y += py * TUX_STEP;

	if (tux->bonus == BONUS_SPEED) {
		new_x += px * TUX_STEP;
		new_y += py * TUX_STEP;
	}

	if (tux->bonus != BONUS_GHOST &&
	   ( isConflictWithObjectFromSpaceBut(arena->spaceTux, new_x, new_y, w, h, tux) ||
	     isConflictModule(new_x, new_y, w, h))) {
		setTuxProportion(tux, zal_x, zal_y);
		return;
	}

	setTuxProportion(tux, zal_x, zal_y);
	moveObjectInSpace(arena->spaceTux, tux, new_x, new_y);
	eventGiveTuxListItem(tux, getCurrentArena()->spaceItem);
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
			addTaskToTimer(getCurrentArena()->listTimer, TIMER_ONE,
					timer_tuxCanSwitchGun, newInt(tux->id),
					TUX_TIME_CAN_SWITCH_GUN);
#ifndef NO_SOUND
			playSound("switch_gun", SOUND_GROUP_BASE);
#endif
			return;
		}
	}

	for (i = 0; i < GUN_COUNT; i++) {
		if (tux->shot[i] > 0) {
			tux->gun = i;
			tux->isCanSwitchGun = FALSE;

			addTaskToTimer(getCurrentArena()->listTimer, TIMER_ONE,
				       timer_tuxCanSwitchGun, newInt(tux->id),
				       TUX_TIME_CAN_SWITCH_GUN);

#ifndef NO_SOUND
			playSound("switch_gun", SOUND_GROUP_BASE);
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
	playSound("switch_gun", SOUND_GROUP_BASE);
#endif

	shotInGun(tux);

	tux->isCanShot = FALSE;

	addTaskToTimer(getCurrentArena()->listTimer, TIMER_ONE, timer_tuxCanShot,
		       newInt(tux->id), TUX_TIME_CAN_SHOT);

	if (tux->shot[tux->gun] == 0) {
		switchTuxGun(tux);
	}
}

void actionTux(tux_t * tux, int action)
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
	if (isTuxAnyGun(tux) == FALSE) {
		tux->gun = GUN_SIMPLE;

		if (tux->pickup_time < TUX_MAX_PICKUP) {
			tux->pickup_time++;
		}

		if (tux->pickup_time == TUX_MAX_PICKUP && getNetTypeGame() != NET_GAME_TYPE_CLIENT) {
#ifndef NO_SOUND
			playSound("switch_gun", SOUND_GROUP_BASE);
#endif

			tux->gun = GUN_SIMPLE;
			tux->shot[tux->gun] = GUN_MAX_SHOT;
			tux->pickup_time = 0;

			if (getNetTypeGame() == NET_GAME_TYPE_SERVER) {
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

		if (tux->bonus_time == 0 && getNetTypeGame() != NET_GAME_TYPE_CLIENT) {
			if (tux->bonus == BONUS_GHOST) {
				int x, y, w, h;
				getTuxProportion(tux, &x, &y, &w, &h);

				tux->bonus = BONUS_NONE;

				if ( /*isConflictTuxWithListTux(tux, getCurrentArena()->listTux) || */
				     isConflictModule(x, y, w, h)) {
					tux->bonus = BONUS_GHOST;
					return;
				}
			}

			tux->bonus = BONUS_NONE;

			if (getNetTypeGame() == NET_GAME_TYPE_SERVER) {
				proto_send_newtux_server(PROTO_SEND_ALL, NULL, tux);
			}
		}
	}
}

void eventTux(tux_t * tux)
{
	arena_t *arena;

	arena = getCurrentArena();

#ifndef PUBLIC_SERVER
	tuxControl(tux);
#endif
	pickUpGun(tux);
	eventBonus(tux);
	eventGiveTuxListItem(tux, arena->spaceItem);
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
		eventTux(thisTux);
	}
}
*/
void getTuxProportion(tux_t * tux, int *x, int *y, int *w, int *h)
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

void setTuxProportion(tux_t * tux, int x, int y)
{
	assert(tux != NULL);

	tux->x = x + TUX_WIDTH / 2;
	tux->y = y + TUX_WIDTH / 2;
}

void getStatusTux(void *p, int *id, int *x, int *y, int *w, int *h)
{
	tux_t *tux;

	tux = p;
	*id = tux->id;
	getTuxProportion(tux, x, y, w, h);
}

void setStatusTux(void *p, int x, int y, int w, int h)
{
	tux_t *tux;

	tux = p;
	setTuxProportion(tux, x, y);
}

static void action_checkMine(space_t * space, item_t * item, tux_t * tux)
{
	if (item->type == ITEM_MINE && item->author_id == tux->id) {
		item->author_id = ID_UNKNOWN;
	}
}

void destroyTux(tux_t * tux)
{
	arena_t *arena;

	arena = getCurrentArena();

	assert(tux != NULL);

	delID(tux->id);

	actionSpace(arena->spaceItem, action_checkMine, tux);

	free(tux);
}

void quitTux()
{
	isTuxInit = FALSE;
}
