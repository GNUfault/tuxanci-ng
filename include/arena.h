
#ifndef ARENA_H

#define ARENA_H

#include "main.h"
#ifndef BUBLIC_SERVER
#include "interface.h"
#endif
#include "list.h"

typedef struct arena_struct
{
#ifndef BUBLIC_SERVER
	char music[STR_SIZE];

	SDL_Surface *background;
#endif

	list_t *listTux;
	list_t *listShot;
	list_t *listWall;
	list_t *listItem;
	list_t *listTeleport;
	list_t *listPipe;
} arena_t;

extern arena_t* newArena();
extern void destroyArena(arena_t *p);

#endif
