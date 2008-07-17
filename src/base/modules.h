
#ifndef MODULE_H

#define MODULE_H

#include "tux.h"
#include "shot.h"
#include "arena.h"
#include "proto.h"
#include "myTimer.h"

#ifndef PUBLIC_SERVER
#include "image.h"
#endif

#define MODULE_TYPE_UNIX	".so"
#define MODULE_TYPE_APPLE	".dylib"
#define MODULE_TYPE_WIN		".dll"

typedef struct export_fce_s
{
	int (*fce_getValue)(char *line, char *env, char *val, int len);

#ifndef PUBLIC_SERVER
	image_t* (*fce_getImage)(char *group, char *name);

	void (*fce_addLayer)(image_t *img,
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
	void (*fce_proto_send_module_server)(int type, client_t *client, char *msg);
	void (*fce_proto_send_module_client)(char *msg);

	my_time_t (*fce_getMyTime)();

	void (*fce_destroyShot)(shot_t *p);
	void (*fce_boundBombBall)(shot_t *shot);
	void (*fce_transformOnlyLasser)(shot_t *shot);
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
	void (*fce_recvMsg)(char *msg);
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
extern int recvMsgModule(char *msg);
extern void quitModule();

#endif 
