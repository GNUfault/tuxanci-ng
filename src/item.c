
#include <stdlib.h>
#include <assert.h>

#include "main.h"
#include "arena.h"
#include "tux.h"
#include "item.h"
#include "shot.h"
#include "proto.h"
#include "net_multiplayer.h"

#ifndef BUBLIC_SERVER
#include "interface.h"
#include "image.h"
#include "sound.h"
#include "layer.h"
#include "screen_world.h"
#include "screen_setting.h"
#include "term.h"
#endif

#ifdef BUBLIC_SERVER
#include "publicServer.h"
#endif

#ifndef BUBLIC_SERVER	
static SDL_Surface *g_item[ITEM_COUNT];
#endif

static bool_t isItemInit = FALSE;

bool_t isItemInicialized()
{
	return isItemInit;
}

void initItem()
{
#ifndef BUBLIC_SERVER	
	g_item[GUN_DUAL_SIMPLE] = addImageData("item.dual.png", IMAGE_ALPHA, "item_dual_simple", IMAGE_GROUP_BASE);
	g_item[GUN_TOMMY] = addImageData("item.tommy.png", IMAGE_ALPHA, "item_tommy_simple", IMAGE_GROUP_BASE);
	g_item[GUN_SCATTER] = addImageData("item.scatter.png", IMAGE_ALPHA, "item_scatter_simple", IMAGE_GROUP_BASE);
	g_item[GUN_LASSER] = addImageData("item.lasser.png", IMAGE_ALPHA, "item_lasser_simple", IMAGE_GROUP_BASE);
	g_item[GUN_MINE] = addImageData("item.mine.png", IMAGE_ALPHA, "item_mines_simple", IMAGE_GROUP_BASE);
	g_item[ITEM_MINE] = addImageData("mine.png", IMAGE_ALPHA, "item_mine_simple", IMAGE_GROUP_BASE);
	g_item[ITEM_EXPLOSION] = addImageData("explosion.png", IMAGE_ALPHA, "item_explosion_simple", IMAGE_GROUP_BASE);
	g_item[ITEM_BIG_EXPLOSION] = addImageData("big-explosion.png", IMAGE_ALPHA, "item_big-explosion_simple", IMAGE_GROUP_BASE);
	
	g_item[BONUS_SPEED] = addImageData("item.bonus.speed.png", IMAGE_ALPHA, "item_bonus_speed", IMAGE_GROUP_BASE);
	g_item[BONUS_SHOT] = addImageData("item.bonus.shot.png", IMAGE_ALPHA, "item_shot_speed", IMAGE_GROUP_BASE);
	g_item[BONUS_TELEPORT] = addImageData("item.bonus.teleport.png", IMAGE_ALPHA, "item_teleport_speed", IMAGE_GROUP_BASE);
	g_item[BONUS_GHOST] = addImageData("item.bonus.ghost.png", IMAGE_ALPHA, "item_ghost_speed", IMAGE_GROUP_BASE);
	g_item[BONUS_4X] = addImageData("item.bonus.4x.png", IMAGE_ALPHA, "item_4x_speed", IMAGE_GROUP_BASE);
	g_item[BONUS_HIDDEN] = addImageData("item.bonus.hidden.png", IMAGE_ALPHA, "item_hidden_speed", IMAGE_GROUP_BASE);
#endif
	isItemInit = TRUE;
}

item_t* newItem(int x, int y, int type, tux_t *author)
{
	item_t *new;
	
	new  = malloc( sizeof(item_t) );
	assert( new != NULL );

	new->type = type;

	new->x = x;
	new->y = y;
	new->frame = 0;
	new->count = 0;
#ifndef BUBLIC_SERVER	
	new->img = g_item[type];
#endif	
	new->author = author;

	switch( type )
	{
		case GUN_DUAL_SIMPLE :
		case GUN_SCATTER :
		case GUN_TOMMY :
		case GUN_LASSER :
		case GUN_MINE :
			new->w = ITEM_GUN_WIDTH;
			new->h = ITEM_GUN_HEIGHT;
		break;

		case ITEM_MINE :
			new->w = ITEM_MINE_WIDTH;
			new->h = ITEM_MINE_HEIGHT;
		break;

		case ITEM_EXPLOSION :
#ifndef BUBLIC_SERVER	
			playSound("explozion", SOUND_GROUP_BASE);
#endif	
			new->w = ITEM_EXPLOSION_WIDTH;
			new->h = ITEM_EXPLOSION_HEIGHT;
		break;

		case ITEM_BIG_EXPLOSION :
#ifndef BUBLIC_SERVER	
			playSound("explozion", SOUND_GROUP_BASE);
#endif	
			new->w = ITEM_BIG_EXPLOSION_WIDTH;
			new->h = ITEM_BIG_EXPLOSION_HEIGHT;
		break;

		case BONUS_SPEED :
		case BONUS_SHOT :
		case BONUS_TELEPORT :
		case BONUS_GHOST :
		case BONUS_4X :
		case BONUS_HIDDEN :
			new->w = ITEM_BONUS_WIDTH;
			new->h = ITEM_BONUS_HEIGHT;
		break;
	}

	return new;
}

void addNewItem(list_t *listItem, tux_t *author)
{
#ifndef BUBLIC_SERVER
	char msg[STR_SIZE];
#endif
	arena_t *arena;
	item_t *item;
	int new_x, new_y;
	int type;

#ifndef BUBLIC_SERVER	
	if( isSettingAnyItem() == FALSE ||
	    getNetTypeGame() == NET_GAME_TYPE_CLIENT )
	{
		return;
	}
#endif

	arena = getWorldArena();
	findFreeSpace(&new_x, &new_y, ITEM_GUN_WIDTH, ITEM_GUN_HEIGHT);

	type = GUN_DUAL_SIMPLE;

#ifndef BUBLIC_SERVER
	do{
#endif
		switch( random() % 11 )
		{
			case 0 : type = GUN_DUAL_SIMPLE;
			break;
			case 1 : type = GUN_SCATTER;
			break;
			case 2 : type = GUN_TOMMY;
			break;
			case 3 : type = GUN_MINE;
			break;
			case 4 : type = GUN_LASSER;
			break;
			case 5 : type = BONUS_SPEED;
			break;
			case 6 : type = BONUS_SHOT;
			break;
			case 7 : type = BONUS_TELEPORT;
			break;
			case 8 : type = BONUS_GHOST;
			break;
			case 9 : type = BONUS_4X;
			break;
			case 10 : type = BONUS_HIDDEN;
			break;
		}
#ifndef BUBLIC_SERVER
	}while( isSettingItem(type) == FALSE );
#endif
	item = newItem(new_x, new_y, type, author);
	addList(listItem, item);

	if( getNetTypeGame() == NET_GAME_TYPE_SERVER )
	{
		proto_send_additem_server(PROTO_SEND_ALL, NULL, item);
	}

#ifndef BUBLIC_SERVER
	sprintf(msg, "in arena is new item\n");
	appendTextInTerm(msg);
#endif

}

#ifndef BUBLIC_SERVER	

void drawItem(item_t *p)
{
	assert( p != NULL );

	addLayer(p->img, p->x, p->y, p->frame * p->w, 0, p->w, p->h, TUX_LAYER);
}

void drawListItem(list_t *listItem)
{
	item_t *thisItem;
	int i;

	assert( listItem != NULL );

	for( i = 0 ; i < listItem->count ; i++ )
	{
		thisItem  = (item_t *)listItem->list[i];
		assert( thisItem != NULL );
		drawItem(thisItem);
	}
}

#endif

void eventListItem(list_t *listItem)
{
	item_t *thisItem;
	int i;

	assert( listItem != NULL );

	for( i = 0 ; i < listItem->count ; i++ )
	{
		thisItem  = (item_t *)listItem->list[i];
		assert( thisItem != NULL );

		thisItem->count++;
	
		if( thisItem->count == ITEM_MAX_COUNT )
		{
			thisItem->count = 0;
			thisItem->frame++;
		}

		switch( thisItem->type )
		{
			case GUN_TOMMY :
			case GUN_DUAL_SIMPLE :
			case GUN_SCATTER  :
			case GUN_LASSER :
			case GUN_MINE :
				if( thisItem->frame == ITEM_GUN_MAX_FRAME )
				{
					thisItem->frame = 0;
				}
			break;

			case ITEM_MINE :
				if( thisItem->frame == ITEM_MINE_MAX_FRAME )
				{
					thisItem->frame = 0;
				}
			break;

			case ITEM_EXPLOSION :
			case ITEM_BIG_EXPLOSION :
				if( thisItem->frame == ITEM_EXPLOSION_MAX_FRAME )
				{
					delListItem(listItem, i, destroyItem);
					i--;
				}
			break;

			case BONUS_SPEED :
			case BONUS_SHOT :
			case BONUS_TELEPORT :
			case BONUS_GHOST :
			case BONUS_4X :
			case BONUS_HIDDEN :
				if( thisItem->frame == ITEM_GUN_MAX_FRAME )
				{
					thisItem->frame = 0;
				}
			break;
		}
	}
}

int isConflictWithListItem(list_t *listItem, int x, int y, int w, int h)
{
	item_t *thisItem;
	int i;

	assert( listItem != NULL );

	for( i = 0 ; i < listItem->count ; i++ )
	{
		thisItem  = (item_t *)listItem->list[i];
		assert( thisItem != NULL );

		if( conflictSpace(x, y, w, h,
		    thisItem->x, thisItem->y, thisItem->w, thisItem->h) )
		{
			return 1;
		}
	}

	return 0;
}

void eventConflictShotWithItem(list_t *listItem, list_t *listShot)
{
	shot_t *thisShot;
	item_t *thisItem;
	int i, j;

	assert( listItem != NULL );
	assert( listShot != NULL );
	
	for( i = 0 ; i < listShot->count ; i++ )
	{
		bool_t isDelShot = FALSE;

		thisShot  = (shot_t *)listShot->list[i];
		assert( thisShot != NULL );

		for( j = 0 ; j < listItem->count ; j++ )
		{
			thisItem  = (item_t *)listItem->list[j];
			assert( thisItem != NULL );

			switch( thisItem->type )
			{
				case ITEM_MINE :
					if( conflictSpace(thisShot->x, thisShot->y, thisShot->w, thisShot->h,
					    thisItem->x, thisItem->y, thisItem->w, thisItem->h) )
					{
						int x, y;
		
						x = ( thisItem->x + thisItem->w/2 ) - ITEM_BIG_EXPLOSION_WIDTH/2;
						y = ( thisItem->y + thisItem->h/2 ) - ITEM_BIG_EXPLOSION_HEIGHT/2;
 						addList(listItem, newItem(x, y, ITEM_BIG_EXPLOSION, thisItem->author) );

						delListItem(listItem, j, destroyItem);
						j--;
					}
				break;

				case ITEM_EXPLOSION :
				case ITEM_BIG_EXPLOSION :
					isDelShot = TRUE;
				break;
			}
		}

		if( isDelShot )
		{
			delListItem(listShot, i, destroyShot);
			i--;
		}
	}
}

static void eventTuxIsDeadWithItem(tux_t *tux, item_t *item)
{
	if( tux->bonus == BONUS_GHOST )
	{
		return;
	}

	if( tux->bonus == BONUS_TELEPORT )
	{
		tuxTeleport(tux);
	}

	if( item->author != NULL && item->author != tux )
	{
#ifndef BUBLIC_SERVER
		char term_msg[STR_SIZE];

		sprintf(term_msg, "tux with id %d set score to %d\n",
			item->author->id, item->author->score+1);
		
		appendTextInTerm(term_msg);
#endif
		item->author->score++;

		if( getNetTypeGame() == NET_GAME_TYPE_SERVER )
		{
			proto_send_score_server(PROTO_SEND_ALL, NULL, item->author);
		}
	}

	countRoundInc();
	eventTuxIsDead(tux);
}

static void tuxGiveBonus(tux_t *tux, item_t *item)
{
#ifndef BUBLIC_SERVER	
	char msg[STR_SIZE];
	playSound("item_bonus", SOUND_GROUP_BASE);
#endif
	tux->bonus = item->type;
	tux->bonus_time = TUX_MAX_BONUS;

#ifndef BUBLIC_SERVER
	sprintf(msg, "tux with id %d bonus enabled\n", tux->id);
	appendTextInTerm(msg);
#endif
}

static void tuxGiveGun(tux_t *tux, item_t *item)
{
#ifndef BUBLIC_SERVER	
	char msg[STR_SIZE];
	playSound("item_gun", SOUND_GROUP_BASE);
#endif

	tux->pickup_time = 0;
	tux->shot[ item->type ] += GUN_MAX_SHOT;
	tux->gun = item->type;

#ifndef BUBLIC_SERVER
	sprintf(msg, "tux with id %d has new gun\n", tux->id);
	appendTextInTerm(msg);
#endif

}

void eventGiveTuxItem(tux_t *tux, list_t *listItem)
{
	item_t *thisItem;
	int x, y, w, h;
	int i;

	assert( listItem != NULL );
	assert( tux != NULL );

	if( tux->status == TUX_STATUS_DEAD )
	{
		return;
	}

	getTuxProportion(tux, &x, &y, &w, &h);
	
	for( i = 0 ; i < listItem->count ; i++ )
	{
		thisItem  = (item_t *)listItem->list[i];
		assert( thisItem != NULL );

		if( conflictSpace(x, y, w, h, thisItem->x, thisItem->y, thisItem->w, thisItem->h) )
		{
			switch( thisItem->type )
			{
				case GUN_TOMMY :
				case GUN_DUAL_SIMPLE :
				case GUN_SCATTER :
				case GUN_LASSER :
				case GUN_MINE :
					tuxGiveGun(tux, thisItem);
					delListItem(listItem, i, destroyItem);
					i--;
				break;

				case ITEM_MINE :
					if( tux->bonus != BONUS_GHOST )
					{
						//eventTuxIsDeadWithItem(tux, thisItem);
						addList(listItem, newItem(x, y,
							ITEM_EXPLOSION, thisItem->author) );

						delListItem(listItem, i, destroyItem);
						i--;
					}
				break;

				case ITEM_EXPLOSION :
				case ITEM_BIG_EXPLOSION :
					eventTuxIsDeadWithItem(tux, thisItem);
				break;

				case BONUS_SPEED :
				case BONUS_SHOT :
				case BONUS_TELEPORT :
				case BONUS_GHOST :
				case BONUS_4X :
				case BONUS_HIDDEN :
					tuxGiveBonus(tux, thisItem);
					delListItem(listItem, i, destroyItem);
					i--;
				break;

			}
		}

	}
}

void destroyItem(item_t *p)
{
	assert( p != NULL );
	free(p);
}

void quitItem()
{
	isItemInit = FALSE;
}
