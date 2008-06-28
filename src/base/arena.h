
#ifndef ARENA_H

#define ARENA_H

#include "base/main.h"

#ifndef PUBLIC_SERVER
#include "client/interface.h"
#endif

#include "base/list.h"
#include "base/space.h"

typedef struct arena_struct
{
#ifndef PUBLIC_SERVER
	char music[STR_SIZE];

	SDL_Surface *background;
#endif
	int w, h;
	list_t *listTimer;
	//list_t *listTux;
	space_t *spaceShot;
	space_t *spaceTux;
	space_t *spaceItem;
} arena_t;

extern void setCurrentArena(arena_t *p);
extern arena_t* getCurrentArena();
extern arena_t* newArena(int w, int h);
extern int conflictSpace(int x1,int y1,int w1,int h1,int x2,int y2,int w2,int h2);
extern int isFreeSpace(arena_t *arena, int x, int y, int w, int h);
extern void findFreeSpace(arena_t *arena, int *x, int *y, int w, int h);
extern void getCenterScreen(int *screen_x, int *screen_y, int x, int y);
#ifndef PUBLIC_SERVER
extern void drawArena(arena_t *arena);
#endif
extern void eventArena(arena_t *arena);
extern void destroyArena(arena_t *p);

#endif
