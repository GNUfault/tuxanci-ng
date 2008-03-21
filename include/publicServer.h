
#ifndef PUBLIC_SERVER_H

#define PUBLIC_SERVER_H

#include "arena.h"

#define WORLD_COUNT_ROUND_UNLIMITED	-1

extern arena_t* getWorldArena();
extern void countRoundInc();
extern int initPublicServer();
extern int getChoiceArenaId();
extern int conflictSpace(int x1,int y1,int w1,int h1,int x2,int y2,int w2,int h2);
extern int isFreeSpace(int x, int y, int w, int h);
extern void findFreeSpace(int *x, int *y, int w, int h);
extern void eventPublicServer();
extern void my_handler_quit(int n);
extern void quitPublicServer();

#endif
