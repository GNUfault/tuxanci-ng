
#ifndef MODULE_H

#define MODULE_H

#include "base/tux.h"
#include "base/shot.h"
#include "base/arena.h"
#include "base/proto.h"
#include "base/myTimer.h"

#ifndef PUBLIC_SERVER
#include "client/image.h"
#endif

typedef struct export_fce_s
{
	int (*fce_getValue)(char *line, char *env, char *val, int len);

#ifndef PUBLIC_SERVER
	SDL_Surface* (*fce_getImage)(char *group, char *name);

	void (*fce_addLayer)(SDL_Surface *img,
	int x,int y, int px,int py,
	int w,int h, int player);
#endif

	int (*fce_getNetTypeGame)();

	void (*fce_getTuxProportion)(tux_t *tux, int *x,int *y, int *w, int *h);
	void (*fce_setTuxProportion)(tux_t *tux, int x, int y);
	tux_t* (*fce_getTuxID)(list_t *listTux, int id);
	void (*fce_actionTux)(tux_t *tux, int action);
	
	arena_t* (*fce_getCurrentArena)();
	int (*fce_conflictSpace)(int x1,int y1,int w1,int h1,int x2,int y2,int w2,int h2);
	int (*fce_isFreeSpace)(arena_t *arena, int x, int y, int w, int h);
	void (*fce_findFreeSpace)(arena_t *arena, int *x, int *y, int w, int h);
	void (*fce_proto_send_newtux_server)(int type, client_t *client, tux_t *tux);
	void (*fce_proto_send_shot_server)(int type, client_t *client, shot_t *p);

	my_time_t (*fce_getMyTime)();

	void (*fce_destroyShot)(shot_t *p);
	void (*fce_boundBombBall)(shot_t *shot);
	void (*fce_transformOnlyLasser)(shot_t *shot);

	int (*fce_pokus)(char *s);

} export_fce_t;

typedef struct module_s
{
	char *file;
	void *image;

	int (*fce_init)(export_fce_t *p);
#ifndef PUBLIC_SERVER
	int (*fce_draw)(int x, int y,int w, int h);
#endif
	int (*fce_event)();
	int (*fce_isConflict)(int x, int y, int w, int h);
	void (*fce_cmd)(char *line);
	int (*fce_destroy)();

} module_t;

extern void initModule();
extern int loadModule(char *name);
#ifndef PUBLIC_SERVER
extern void drawModule(int x, int y, int w, int h);
#endif
extern void eventModule();
extern void cmdModule(char *s);
extern int isConflictModule(int x, int y, int w, int h);
extern void quitModule();

#endif 
