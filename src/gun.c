
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "arena.h"
#include "tux.h"
#include "shot.h"
#include "dynamicInt.h"
#include "myTimer.h"
#include "main.h"
#include "item.h"
#include "sound.h"
#include "screen_world.h"
#include "interface.h"
#include "gun.h"

static void modificiationCopuse(int courser,
	int right_x, int right_y, int right_px, int right_py,
	int *dest_x, int *dest_y, int *dest_px, int *dest_py)
{
	assert( dest_px != NULL );
	assert( dest_py != NULL );

	switch( courser )
	{
		case TUX_UP :
			*dest_x = right_y;
			*dest_y = -right_x;
			*dest_px = right_py;
			*dest_py = -right_px;
		break;
		case TUX_RIGHT :
			*dest_x = right_x;
			*dest_y = right_y;
			*dest_px = right_px;
			*dest_py = right_py;
		break;
		case TUX_LEFT :
			*dest_x = -right_x;
			*dest_y = right_y;
			*dest_px = -right_px;
			*dest_py = right_py;
		break;
		case TUX_DOWN :
			*dest_x = right_y;
			*dest_y = right_x;
			*dest_px = right_py;
			*dest_py = right_px;
		break;
		default :
			assert( ! "V premennej courser je zly smer !" );
		break;
	}
}

static void addShot(tux_t *tux,int x, int y, int px, int py)
{
	int dest_x, dest_y;
	int dest_px, dest_py;
	arena_t *arena;
	shot_t *shot;
	int gun;

	arena = getWorldArena();

	if( tux->gun == GUN_LASSER )
	{
		gun = GUN_LASSER;
	}
	else
	{
		gun = GUN_SIMPLE;
	}

	if( tux->bonus == BONUS_4X )
	{
		int zal;
		int i;

		zal = tux->position;

		for( i = 0 ; i < 4 ; i++ )
		{
			switch( i )
			{
				case 0 : tux->position = TUX_UP; break;
				case 1 : tux->position = TUX_RIGHT; break;
				case 2 : tux->position = TUX_LEFT; break;
				case 3 : tux->position = TUX_DOWN; break;
			}

			modificiationCopuse(tux->position, x, y, px, py, 
				&dest_x, &dest_y, &dest_px, &dest_py);

			shot = newShot(tux->x + dest_x, tux->y + dest_y, dest_px, dest_py, gun, tux);
			addList( arena->listShot, shot );
		}

		tux->position = zal;
	}
	else
	{
		modificiationCopuse(tux->position, x, y, px, py, 
			&dest_x, &dest_y, &dest_px, &dest_py);

		shot = newShot(tux->x + dest_x, tux->y + dest_y, dest_px, dest_py, gun, tux);
		addList( arena->listShot, shot );
	}
}

static void shotInGunSimpe(tux_t *tux)
{
	addShot(tux, 0, 0, +10, 0);
}

static void shotInGunDualSimpe(tux_t *tux)
{
	addShot(tux, 0, -5, +10, 0);
	addShot(tux, 0, +5, +10, 0);
}

static void shotInGunScatter(tux_t *tux)
{
	int i;

	for( i = -2 ; i <= +2 ; i++ )
	{
		addShot(tux, 0, 0, +12, i);
	}
}

static void timer_addShotTimer(void *p)
{
	tux_t *tux;
	int id;
	int gun;

	id =  * ((int *)p);
	free(p);

	tux = getTuxID(getWorldArena()->listTux, id);

	if( tux == NULL )return;

	gun = tux->gun;
	tux->gun = GUN_SIMPLE;
	addShot(tux, 0, 0, +15, 0);
	tux->gun = gun;
}

static void shotInGunTommy(tux_t *tux)
{
	int i;

	for( i = 0 ; i < 10 ; i++ )
	{
		addTimer(timer_addShotTimer, newInt(tux->id), i * 100 );
	}
}

static void timer_addLaserTimer(void *p)
{
	tux_t *tux;
	int id;
	int gun;

	id =  * ((int *)p);
	free(p);

	tux = getTuxID(getWorldArena()->listTux, id);

	if( tux == NULL )return;

	gun = tux->gun;
	tux->gun = GUN_LASSER;
	addShot(tux, 0, 0, +20, 0);
	tux->gun = gun;
}

static void shotInGunLasser(tux_t *tux)
{
	int i;

	for( i = 0 ; i < 50 ; i++ )
	{
		addTimer(timer_addLaserTimer, newInt(tux->id), i * 10 );
	}
}

static void putInGunMine(tux_t *tux)
{
	int x, y;
	arena_t *arena;

	x = tux->x;
	y = tux->y;

	switch( tux->position )
	{
		case TUX_UP :
			y += TUX_HEIGHT;
		break;
		case TUX_RIGHT :
			x -= TUX_WIDTH;
		break;
		case TUX_LEFT :
			x += TUX_WIDTH;
		break;
		case TUX_DOWN :
			y -= TUX_HEIGHT;
		break;
	}

	arena = getWorldArena();

	if( isFreeSpace(x, y, ITEM_MINE_WIDTH, ITEM_MINE_HEIGHT) )
	{
		addList(arena->listItem, newItem(x, y, ITEM_MINE) );
	}
}

void shotInGun(tux_t *tux)
{
	switch( tux->gun )
	{
		case GUN_SIMPLE :
			playSound("gun_revolver", SOUND_GROUP_BASE);
			shotInGunSimpe(tux);
		break;
		case GUN_DUAL_SIMPLE :
			playSound("gun_revolver", SOUND_GROUP_BASE);
			shotInGunDualSimpe(tux);
		break;
		case GUN_SCATTER :
			playSound("gun_scatter", SOUND_GROUP_BASE);
			shotInGunScatter(tux);
		break;
		case GUN_TOMMY :
			playSound("gun_tommy", SOUND_GROUP_BASE);
			shotInGunTommy(tux);
		break;
		case GUN_LASSER :
			playSound("gun_lasser", SOUND_GROUP_BASE);
			shotInGunLasser(tux);
		break;
		case GUN_MINE :
			playSound("put_mine", SOUND_GROUP_BASE);
			putInGunMine(tux);
		break;
	}
}
