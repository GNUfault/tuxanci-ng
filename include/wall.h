
#ifndef WALL_H

#define WALL_H

#ifndef BUBLIC_SERVER
#include "interface.h"
#endif

#include "list.h"

typedef struct wall_struct
{
	int x; // poloha steny	
	int y;

	int w; // rozmery steny
	int h;

	int img_x; // poloha obrazka
	int img_y;

	int layer; // vrstva

#ifndef BUBLIC_SERVER	
	SDL_Surface *img; //obrazok
#endif
} wall_t;

#ifndef BUBLIC_SERVER	
extern wall_t* newWall(int x, int y, int w, int h,
	int img_x, int img_y, int layer, SDL_Surface *img);
#endif

#ifdef BUBLIC_SERVER	
extern wall_t* newWall(int x, int y, int w, int h,
	int img_x, int img_y, int layer);
#endif

extern void drawWall(wall_t *p);
extern void drawListWall(list_t *listWall);
extern int isConflictWithListWall(list_t *listWall, int x, int y, int w, int h);
extern void eventConflictShotWithWall(list_t *listWall, list_t *listShot);
extern void destroyWall(wall_t *p);

#endif

