
#ifndef TELEPORT_H

#define TELEPORT_H

#ifndef BUBLIC_SERVER
#include "interface.h"
#endif
#include "main.h"
#include "list.h"

typedef struct teleport_struct
{
	int x; // poloha steny	
	int y;

	int w; // rozmery steny
	int h;

	int layer; // vrstva

#ifndef BUBLIC_SERVER	
	SDL_Surface *img; //obrazok
#endif
} teleport_t;

#ifndef BUBLIC_SERVER	
extern teleport_t* newTeleport(int x, int y, int w, int h, int layer, SDL_Surface *img);
#endif

#ifdef BUBLIC_SERVER	
extern teleport_t* newTeleport(int x, int y, int w, int h, int layer);
#endif

#ifndef BUBLIC_SERVER	
extern void drawTeleport(teleport_t *p);
extern void drawListTeleport(list_t *listTeleport);
#endif
extern teleport_t* isConflictWithListTeleport(list_t *listTeleport, int x, int y, int w, int h);
extern void eventConflictShotWithTeleport(list_t *listTeleport, list_t *listShot);
extern void eventTeleportTux(list_t *listTeleport, teleport_t *teleport, tux_t *tux);
extern void destroyTeleport(teleport_t *p);

#endif

