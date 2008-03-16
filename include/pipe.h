
#ifndef PIPE_H

#define PIPE_H

#include "main.h"
#ifndef BUBLIC_SERVER
#include "interface.h"
#endif
#include "list.h"

typedef struct pipe_struct
{
	int x; // poloha steny	
	int y;

	int w; // rozmery steny
	int h;

	int id;
	int id_out;
	int position;

	int layer; // vrstva

#ifndef BUBLIC_SERVER	
	SDL_Surface *img; //obrazok
#endif
} pipe_t;

#ifndef BUBLIC_SERVER	
extern pipe_t* newPipe(int x, int y, int w, int h, int layer,
		int id, int id_out, int position, SDL_Surface *img);
#endif

#ifdef BUBLIC_SERVER	
extern pipe_t* newPipe(int x, int y, int w, int h, int layer,
		int id, int id_out, int position);
#endif

#ifndef BUBLIC_SERVER	
extern void drawPipe(pipe_t *p);
extern void drawListPipe(list_t *listPipe);
#endif
extern pipe_t* isConflictWithListPipe(list_t *listPipe, int x, int y, int w, int h);
extern void eventConflictShotWithPipe(list_t *listPipe, list_t *listShot);
extern void destroyPipe(pipe_t *p);

#endif

