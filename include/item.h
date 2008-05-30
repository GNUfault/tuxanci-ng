
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
extern item_t* getItemID(list_t *listItem, int id);
extern void replaceItemID(item_t *item, int id);
extern void addNewItem(list_t *listItem, int author_id);
#ifndef PUBLIC_SERVER	
extern void drawItem(item_t *p);
extern void drawListItem(list_t *listItem);
#endif
extern void eventListItem(list_t *listItem);
extern void eventConflictShotWithItem(list_t *listItem, list_t *listShot);
extern int isConflictWithListItem(list_t *listItem, int x, int y, int w, int h);
extern void mineExplosion(list_t *listItem, item_t *item);
extern int eventGiveTuxItem(tux_t *tux, list_t *listItem, item_t *item);
extern void eventGiveTuxListItem(tux_t *tux, list_t *listItem);
extern void destroyItem(item_t *p);
extern void quitItem();

#endif

