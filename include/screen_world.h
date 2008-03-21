
#ifndef SCREEN_WORLD_H

#define SCREEN_WORLD_H

#include "main.h"
#include "tux.h"
#include "arena.h"

#define TIMER_END_ARENA 5000
#define WORLD_COUNT_ROUND_UNLIMITED	-1

extern bool_t isScreenWorldInicialized();
extern void initWorld();
extern arena_t* getWorldArena();
extern void setWorldArena(int id);
extern void countRoundInc();
extern void setWorldEnd();
extern void setMaxCountRound(int n);
extern int conflictSpace(int x1,int y1,int w1,int h1,int x2,int y2,int w2,int h2);
extern int isFreeSpace(int x, int y, int w, int h);
extern void findFreeSpace(int *x, int *y, int w, int h);
extern void tuxControl(tux_t *p);
extern void drawWorld();
extern void eventWorld();
extern void startArena();
extern void stopArena();
extern void quitWorld();

#endif
