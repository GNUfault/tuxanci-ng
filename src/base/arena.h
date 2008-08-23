
#ifndef ARENA_H

#define ARENA_H

#include "main.h"

#ifndef PUBLIC_SERVER
#include "image.h"
#endif

#ifdef __WIN32__
#define random rand
#endif

#define SCREEN_SPLIT_HORIZONTAL		1
#define SCREEN_SPLIT_VERTICAL		2

#include "list.h"
#include "space.h"

typedef struct arena_struct
{
#ifndef PUBLIC_SERVER
	char music[STR_SIZE];

	image_t *background;
#endif
	int w, h;

	list_t *listTimer;

	space_t *spaceShot;
	space_t *spaceTux;
	space_t *spaceItem;

	int countRound;
	int max_countRound;

} arena_t;

extern void setCurrentArena(arena_t *p);
extern arena_t* getCurrentArena();
extern void initArena();
extern void quitArena();
extern arena_t* newArena(int w, int h);
extern int conflictSpace(int x1,int y1,int w1,int h1,int x2,int y2,int w2,int h2);
extern int isFreeSpace(arena_t *arena, int x, int y, int w, int h);
extern void findFreeSpace(arena_t *arena, int *x, int *y, int w, int h);
extern void getCenterScreen(int *screen_x, int *screen_y, int x, int y, int screen_size_x, int screen_size_y);
#ifndef PUBLIC_SERVER
extern void drawArena(arena_t *arena);
#endif
extern void eventArena(arena_t *arena);
extern void destroyArena(arena_t *p);

#endif
