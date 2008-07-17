
#ifndef SHOT_H

#define SHOT_H

#include "main.h"
#include "list.h"
#include "tux.h"
#include "arena.h"

#ifdef __WIN32__
#define random rand
#endif

#ifndef PUBLIC_SERVER	
#include "image.h"
#endif	

typedef struct shot_struct
{
	int id;

	int x;
	int y;

	int w;
	int h;

	int px;
	int py;

	int position;
	int gun;

	int author_id;
	bool_t isCanKillAuthor;

#ifndef PUBLIC_SERVER	
	image_t *img;
#endif	
} shot_t; 

extern bool_t isShotInicialized();
extern void initShot();
extern shot_t* newShot(int x,int y, int px, int py, int gun, int author_id);
extern shot_t* getShotID(list_t *listShot, int id);
extern void replaceShotID(shot_t *shot, int id);
extern void getStatusShot(void *p, int *id, int *x,int *y, int *w, int *h);
extern void setStatusShot(void *p, int x, int y, int w, int h);
#ifndef PUBLIC_SERVER	
extern void drawShot(shot_t *p);
extern void drawListShot(list_t *listShot);
#endif
extern int isConflictWithListShot(list_t *listShot, int x, int y, int w, int h);
extern void eventMoveListShot(arena_t *arena);
extern void checkShotIsInTuxScreen(arena_t *arena);

extern void boundBombBall(shot_t *shot);
extern void transformOnlyLasser(shot_t *shot);

extern void destroyShot(shot_t *p);
extern void quitShot();

#endif

