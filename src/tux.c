
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "main.h"
#include "tux.h"
#include "shot.h"
#include "gun.h"
#include "wall.h"
#include "teleport.h"
#include "pipe.h"
#include "item.h"
#include "arena.h"
#include "myTimer.h"
#include "net_multiplayer.h"
#include "dynamicInt.h"
#include "proto.h"

#ifndef BUBLIC_SERVER
#include "image.h"
#include "sound.h"
#include "layer.h"
#include "screen_world.h"
#endif

#ifdef BUBLIC_SERVER
#include "publicServer.h"
#endif

#ifndef BUBLIC_SERVER

static SDL_Surface *g_tux_up;
static SDL_Surface *g_tux_right;
static SDL_Surface *g_tux_left;
static SDL_Surface *g_tux_down;
static SDL_Surface *g_cross;

#endif

static bool_t isTuxInit = FALSE;

bool_t isTuxInicialized()
{
	return isTuxInit;
}

void initTux()
{
#ifndef BUBLIC_SERVER
	assert( isImageInicialized() == TRUE );
#endif

#ifndef BUBLIC_SERVER
	g_tux_up = addImageData("tux8.png", IMAGE_ALPHA, "tux8", IMAGE_GROUP_BASE);
	g_tux_right = addImageData("tux6.png", IMAGE_ALPHA, "tux6", IMAGE_GROUP_BASE);
	g_tux_left = addImageData("tux4.png", IMAGE_ALPHA, "tux4", IMAGE_GROUP_BASE);
	g_tux_down = addImageData("tux2.png", IMAGE_ALPHA, "tux2", IMAGE_GROUP_BASE);
	g_cross = addImageData("cross.png", IMAGE_ALPHA, "cross", IMAGE_GROUP_BASE);
#endif

	isTuxInit = TRUE;
}

tux_t* newTux()
{
	static int last_id = 0;
	int x, y;
	tux_t *new;

	new = malloc( sizeof(tux_t) );
	assert( new != NULL );

	memset(new, 0, sizeof(tux_t) );
	
	new->id = last_id++;
	new->status = TUX_STATUS_ALIVE;
	new->control = TUX_CONTROL_NONE;

	findFreeSpace(&x, &y, TUX_WIDTH, TUX_HEIGHT);
	setTuxProportion(new, x, y);

	new->position = TUX_DOWN;
	new->gun = GUN_SIMPLE;
	new->shot[ new->gun ] = GUN_MAX_SHOT;
	
	sprintf(new->name, "no_name_id_%d", new->id);
	new->score = 0;
	new->frame = 0;

	new->bonus = BONUS_NONE;
	new->bonus_time = 0;

	new->isCanShot = TRUE;
	new->isCanSwitchGun = TRUE;

	return new;
}

bool_t isTuxAnyGun(tux_t *tux)
{
	int i;
	
	for( i = 0 ; i < GUN_COUNT ; i++ )
	{
		if( tux->shot[i] > 0 )
		{
			return TRUE;
		}
	}

	return FALSE;
}

void getCourse(int n, int *x, int *y)
{
	assert( x != NULL );
	assert( y != NULL );

	switch( n )
	{
		case TUX_UP :
			*x = 0;
			*y = -1;
		break;

		case TUX_LEFT :
			*x = -1;
			*y = 0;
		break;

		case TUX_RIGHT :
			*x = +1;
			*y = 0;
		break;

		case TUX_DOWN :
			*x = 0;
			*y = +1;
		break;

		default :
			assert( ! "premenna n ma zlu hodnotu !" );
		break;
	}	
}

#ifndef BUBLIC_SERVER

void drawTux(tux_t *tux)
{
	SDL_Surface *g_image = NULL;

	assert( tux != NULL );

	if( tux->bonus == BONUS_HIDDEN && 
	    getNetTypeGame() != NET_GAME_TYPE_NONE &&
	    tux->control == TUX_CONTROL_NET )
	{
		return;
	}

	switch( tux->position )
	{
		case TUX_UP :
			g_image = g_tux_up;
		break;

		case TUX_LEFT :
			g_image = g_tux_left;
		break;

		case TUX_RIGHT :
			g_image = g_tux_right;
		break;

		case TUX_DOWN :
			g_image = g_tux_down;
		break;

		default :
			assert( ! "premenna p->control ma zlu hodnotu !" );
		break;
	}

	if( tux->status == TUX_STATUS_DEAD )
	{
		g_image = g_cross;

		addLayer(g_cross,
			tux->x - g_cross->w / 2,
			( tux->y + g_cross->h / 2 ) - g_cross->h,
			0, 0,
			g_cross->w, g_cross->h, TUX_LAYER);

		return;
	}

	addLayer(g_image,
		tux->x - TUX_IMG_WIDTH / 2,
		( tux->y + TUX_HEIGHT / 2 ) - TUX_IMG_HEIGHT,
		tux->frame * TUX_IMG_WIDTH, 0,
		TUX_IMG_WIDTH, TUX_IMG_HEIGHT, TUX_LAYER);
}

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

#endif

tux_t* getTuxID(list_t *listTux, int id)
{
	tux_t *thisTux;
	int i;

	assert( listTux != NULL );

	for( i = 0 ; i < listTux->count ; i++ )
	{
		thisTux  = (tux_t *)listTux->list[i];
		assert( thisTux != NULL );

		if( thisTux->id == id )
		{
			return thisTux;
		}
	}

	return NULL;
}

tux_t* isConflictWithListTux(list_t *listTux, int x, int y, int w, int h)
{
	tux_t *thisTux;
	int tx, ty, tw, th;
	int i;

	assert( listTux != NULL );

	for( i = 0 ; i < listTux->count ; i++ )
	{
		thisTux  = (tux_t *)listTux->list[i];
		assert( thisTux != NULL );

		getTuxProportion(thisTux, &tx, &ty, &tw, &th);
		
		if( conflictSpace(x, y, w, h, tx, ty, tw, th) )
		{
			return thisTux;
		}
	}

	return NULL;
}

int isConflictTuxWithListTux(tux_t *tux, list_t *listTux)
{
	tux_t *thisTux;
	int tux_x, tux_y, tux_w, tux_h;
	int thisTux_x, thisTux_y, thisTux_w, thisTux_h;
	int i;

	assert( tux != NULL );
	assert( listTux != NULL );

	getTuxProportion(tux, &tux_x, &tux_y, &tux_w, &tux_h);

	for( i = 0 ; i < listTux->count ; i++ )
	{
		thisTux  = (tux_t *)listTux->list[i];
		assert( thisTux != NULL );

		if( thisTux == tux )
		{
			continue;
		}

		getTuxProportion(thisTux, &thisTux_x, &thisTux_y, &thisTux_w, &thisTux_h);
		
		if( conflictSpace(tux_x, tux_y, tux_w, tux_h,
		    thisTux_x, thisTux_y, thisTux_w, thisTux_h) )
		{
			return 1;
		}
	}

	return 0;
}

static void timer_spawnTux(void *p)
{
	tux_t *tux;
	arena_t *arena;
	int x, y;
	int id;

	id =  * ((int *)p);
	free(p);

	arena = getWorldArena();
	tux = getTuxID(arena->listTux, id);

	if( tux == NULL )return;

	tux->status = TUX_STATUS_ALIVE;
	findFreeSpace(&x, &y, TUX_WIDTH, TUX_HEIGHT);
	setTuxProportion(tux, x, y);
	tux->gun = GUN_SIMPLE;
	
	addNewItem(arena->listItem, NULL);

	if( getNetTypeGame() == NET_GAME_TYPE_SERVER )
	{
		proto_send_newtux_server(NULL, tux);
	}
}

static void timer_tuxCanShot(void *p)
{
	tux_t *tux;
	int id;

	id =  * ((int *)p);
	free(p);

	tux = getTuxID(getWorldArena()->listTux, id);

	if( tux == NULL )return;

	tux->isCanShot = TRUE;
}

static void timer_tuxCanSwitchGun(void *p)
{
	tux_t *tux;
	int id;

	id =  * ((int *)p);
	free(p);

	tux = getTuxID(getWorldArena()->listTux, id);

	if( tux == NULL )return;

	tux->isCanSwitchGun = TRUE;
}

void eventTuxIsDead(tux_t *tux)
{
	if( tux->status == TUX_STATUS_DEAD )
	{
		return;
	}

#ifndef BUBLIC_SERVER
	playSound("dead", SOUND_GROUP_BASE);
#endif
	printf("tux id %d is dead\n", tux->id);

	tux->status = TUX_STATUS_DEAD;
	memset(tux->shot, 0, sizeof(int) * GUN_COUNT);
	tux->gun = GUN_SIMPLE;
	tux->shot[ tux->gun ] = GUN_MAX_SHOT;

	tux->bonus = BONUS_NONE;
	tux->bonus_time = 0;
	tux->pickup_time = 0;

	tux->isCanShot = TRUE;
	tux->isCanSwitchGun = TRUE;

	if( getNetTypeGame() != NET_GAME_TYPE_CLIENT )
	{
		addTimer(timer_spawnTux, newInt(tux->id), TUX_TIME_SPAWN );
	}

	if( getNetTypeGame() == NET_GAME_TYPE_SERVER )
	{
		proto_send_kill_server(tux);
	}
}

static void eventTuxIsDeadWIthShot(tux_t *tux, shot_t *shot)
{
	if( shot->author != NULL && shot->author != tux )
	{
		shot->author->score++;
	}

	if( getNetTypeGame() == NET_GAME_TYPE_SERVER )
	{
		proto_send_score_server(shot->author);
	}

	countRoundInc();
	eventTuxIsDead(tux);
}

void tuxTeleport(tux_t *tux)
{
	int x, y;

#ifndef BUBLIC_SERVER
	playSound("teleport", SOUND_GROUP_BASE);
#endif
	printf("tux id %d teleporting\n", tux->id);

	if( getNetTypeGame() != NET_GAME_TYPE_CLIENT )
	{
		findFreeSpace(&x, &y, TUX_WIDTH, TUX_HEIGHT);
		setTuxProportion(tux, x, y);
	}

	if( getNetTypeGame() == NET_GAME_TYPE_SERVER )
	{
		proto_send_newtux_server(NULL, tux);
	}
}

void eventConflictTuxWithShot(list_t *listTux, list_t *listShot)
{
	shot_t *thisShot;
	tux_t *thisTux;

	int i;

	assert( listTux != NULL );
	assert( listShot != NULL );

	for( i = 0 ; i < listShot->count ; i++ )
	{
		thisShot = (shot_t *)listShot->list[i];
		assert( thisShot != NULL );

		if( ( thisTux = isConflictWithListTux(listTux, thisShot->x, thisShot->y,
			thisShot->w, thisShot->h) ) != NULL ) 
		{
			if(  thisTux->status == TUX_STATUS_ALIVE )
			{
				if( thisShot->author == thisTux &&
				    thisShot->isCanKillAuthor == FALSE )
				{
					continue;
				}

				if( thisTux->bonus == BONUS_TELEPORT )
				{
					tuxTeleport(thisTux);
					continue;
				}

				if( getNetTypeGame() != NET_GAME_TYPE_CLIENT )
				{
					eventTuxIsDeadWIthShot(thisTux, thisShot);
				}
			}

			delListItem(listShot, i, destroyShot);
			i--;

			continue;
		}
	}
}

void eventConflictTuxWithTeleport(list_t *listTux, list_t *listTeleport)
{
	teleport_t *thisTeleport;
	tux_t *thisTux;

	int i;

	assert( listTux != NULL );
	assert( listTeleport != NULL );

	for( i = 0 ; i < listTeleport->count ; i++ )
	{
		thisTeleport = (teleport_t *)listTeleport->list[i];
		assert( thisTeleport != NULL );

		if( ( thisTux = isConflictWithListTux(listTux, thisTeleport->x, thisTeleport->y,
			thisTeleport->w, thisTeleport->h) ) != NULL )
		{
			if( getNetTypeGame() != NET_GAME_TYPE_CLIENT )
			{
				eventTeleportTux(listTeleport, thisTeleport, thisTux);
			}
		}
	}
}

void moveTux(tux_t *tux, int n)
{
	int px, py;
	int zal_x, zal_y;
	int x, y, w, h;
	arena_t *arena;

	assert( tux != NULL );

	if( tux->position != n )
	{
		tux->position = n;
		return;
	}

	if( n == TUX_LEFT && tux->x < 0 )
	{
		return;
	}

	if( n == TUX_RIGHT && tux->x > WINDOW_SIZE_X )
	{
		return;
	}

	if( n == TUX_UP && tux->y < 0 )
	{
		return;
	}

	if( n == TUX_DOWN && tux->y > WINDOW_SIZE_Y )
	{
		return;
	}

	arena = getWorldArena();
	getCourse(tux->position, &px, &py);
	
	zal_x = tux->x;
	zal_y = tux->y;

	tux->x += px * TUX_STEP;
	tux->y += py * TUX_STEP;

	if( tux->bonus == BONUS_SPEED )
	{
		tux->x += px * TUX_STEP;
		tux->y += py * TUX_STEP;
	}

	getTuxProportion(tux, &x, &y, &w, &h);


	if( tux->bonus != BONUS_GHOST && (
	    isConflictTuxWithListTux(tux, arena->listTux) ||
	    isConflictWithListWall(arena->listWall, x, y, w, h) ||
	    isConflictWithListPipe(arena->listPipe, x, y, w, h) ) )
	{
		tux->x = zal_x;
		tux->y = zal_y;
	}
	else
	{
		tux->frame++;

		if( tux->frame == TUX_MAX_ANIMATION_FRAME ) tux->frame = 0;
	}
}

void switchTuxGun(tux_t *tux)
{
	int i;
	
	if( tux->isCanSwitchGun == FALSE )
	{
		return;
	}

#ifndef BUBLIC_SERVER
	playSound("switch_gun", SOUND_GROUP_BASE);
#endif
	for( i = tux->gun+1 ; i < GUN_COUNT ; i++ )
	{
		if( tux->shot[i] > 0 )
		{
			tux->gun = i;
			tux->isCanSwitchGun = FALSE;
			addTimer(timer_tuxCanSwitchGun, newInt(tux->id), TUX_TIME_CAN_SWITCH_GUN );
			return;
		}
	}

	for( i = 0 ; i < GUN_COUNT ; i++ )
	{
		if( tux->shot[i] > 0 )
		{
			tux->gun = i;
			tux->isCanSwitchGun = FALSE;
			addTimer(timer_tuxCanSwitchGun, newInt(tux->id), TUX_TIME_CAN_SWITCH_GUN );
			return;
		}
	}
}

void shotTux(tux_t *tux)
{
	assert( tux != NULL );

	if( tux->isCanShot == FALSE  ||
	    tux->shot[tux->gun] == 0 )
	{
		return;
	}

	shotInGun(tux);

	tux->isCanShot = FALSE;
	addTimer(timer_tuxCanShot, newInt(tux->id), TUX_TIME_CAN_SHOT );

	if( tux->shot[tux->gun] == 0 )
	{
		switchTuxGun(tux);
	}
}

void actionTux(tux_t *tux, int action)
{
	if( tux->status != TUX_STATUS_ALIVE )
	{
		return;
	}

	switch( action )
	{
		case TUX_UP :
		case TUX_LEFT :
		case TUX_RIGHT :
		case TUX_DOWN :
			moveTux(tux, action);
		break;

		case TUX_SHOT :
			shotTux(tux);
		break;

		case TUX_SWITCH_GUN :
			switchTuxGun(tux);
		break;
	}
}

static void pickUpGun(tux_t *tux)
{
	if( isTuxAnyGun(tux) == FALSE )
	{
		tux->gun = GUN_SIMPLE;
		tux->pickup_time++;

		if( tux->pickup_time == TUX_MAX_PICKUP )
		{
			tux->gun = GUN_SIMPLE;
			tux->shot[ tux->gun ] = GUN_MAX_SHOT;
			tux->pickup_time = 0;
		}
	}
}

static void eventBonus(tux_t *tux)
{
	if( tux->bonus != BONUS_NONE )
	{
		if( tux->bonus_time > 0 )
		{
			tux->bonus_time--;
		}

		if( tux->bonus_time == 0 )
		{			
			if( tux->bonus == BONUS_GHOST )
			{
				int x, y, w, h;
				getTuxProportion(tux, &x, &y, &w, &h);

				if ( isConflictWithListWall(getWorldArena()->listWall, x, y, w, h) )
				{
					return;
				}
			}

			tux->bonus = BONUS_NONE;

			if( getNetTypeGame() == NET_GAME_TYPE_SERVER )
			{
				proto_send_newtux_server(NULL, tux);
			}
		}
	}
}

void eventListTux(list_t *listTux)
{
	tux_t *thisTux;
	int i;

	assert( listTux != NULL );

	for( i = 0 ; i < listTux->count ; i++ )
	{
		thisTux  = (tux_t *)listTux->list[i];
		assert( thisTux != NULL );

#ifndef BUBLIC_SERVER
		tuxControl(thisTux);
#endif
		pickUpGun(thisTux);
		eventBonus(thisTux);
		eventGiveTuxItem(thisTux, getWorldArena()->listItem);
	}
}

void getTuxProportion(tux_t *tux, int *x,int *y, int *w, int *h)
{
	assert( tux != NULL );

	if( x != NULL ) *x = tux->x - TUX_WIDTH / 2;
	if( y != NULL ) *y = tux->y - TUX_HEIGHT / 2;
	if( w != NULL ) *w = TUX_WIDTH;
	if( h != NULL ) *h = TUX_HEIGHT;
}

void setTuxProportion(tux_t *tux, int x, int y)
{
	assert( tux != NULL );

	tux->x = x + TUX_WIDTH / 2;
	tux->y = y + TUX_WIDTH / 2;
}

void destroyTux(tux_t *tux)
{
	assert( tux != NULL );
	free(tux);
}

void quitTux()
{
	isTuxInit = FALSE;
}

