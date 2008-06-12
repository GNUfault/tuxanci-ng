
#ifndef ITEM_H

#define ITEM_H

#include "main.h"

#define ITEM_SYNC_TIMEOUT	5000

#ifndef PUBLIC_SERVER
#include "interface.h"
#endif

#include "list.h"
#include "tux.h"
#include "myTimer.h"

typedef struct item_struct
{
	int id;
	int type; // typ veci

	int x; // poloha veci	
	int y;

	int w; // rozmery veci
	int h;

	int frame; //poradove cislo animacie
	int count; 

	int author_id;
#ifndef PUBLIC_SERVER	
	SDL_Surface *img; //obrazok
#endif	
} item_t;

#define ITEM_MAX_COUNT	1

#define ITEM_GUN_MAX_FRAME		8
#define ITEM_MINE_MAX_FRAME		1
#define ITEM_EXPLOSION_MAX_FRAME	10
#define ITEM_BONUS_MAX_FRAME		8

#define ITEM_GUN_WIDTH	50
#define ITEM_GUN_HEIGHT	20	

#define ITEM_MINE_WIDTH		15
#define ITEM_MINE_HEIGHT	11

#define ITEM_EXPLOSION_WIDTH	40
#define ITEM_EXPLOSION_HEIGHT	40

#define ITEM_BIG_EXPLOSION_WIDTH	100
#define ITEM_BIG_EXPLOSION_HEIGHT	100

#define ITEM_BONUS_WIDTH	20
#define ITEM_BONUS_HEIGHT	20

extern bool_t isItemInicialized();
extern void initItem();
extern item_t* newItem(int x, int y, int type, int author_id);
extern void getStatusItem(void *p, int *id, int *x, int *y ,int *w, int *h);
extern void setStatusItem(void *p, int x, int y, int w, int h);
extern void replaceItemID(item_t *item, int id);
extern void addNewItem(space_t *spaceItem, int author_id);
#ifndef PUBLIC_SERVER	
extern void drawListItem(list_t *listItem);
#endif
extern void eventListItem(space_t *listSpace);
extern void mineExplosion(space_t *spaceItem, item_t *item);
extern void eventConflictShotWithItem(arena_t *arena);
extern void eventGiveTuxItem(tux_t *tux, item_t *item, space_t *spaceItem);
extern void eventGiveTuxListItem(tux_t *tux, space_t *spaceItem);
extern void destroyItem(item_t *p);
extern void quitItem();

#endif

