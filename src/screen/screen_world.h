
#ifndef SCREEN_WORLD_H

#define SCREEN_WORLD_H

#include "base/main.h"
#include "base/myTimer.h"
#include "base/tux.h"
#include "base/arena.h"

#define TIMER_END_ARENA 5000
#define WORLD_COUNT_ROUND_UNLIMITED	-1
#define LAG_SERVER_UNKNOWN		-1

extern bool_t isScreenWorldInicialized();
extern void initWorld();
extern void setWorldArena(int id);
extern void setLagServer(my_time_t lag);
extern void countRoundInc();
extern void setWorldEnd();
extern void setMaxCountRound(int n);
extern tux_t* getControlTux(int control_type);
extern void setControlTux(tux_t *tux, int control_type);
extern void tuxControl(tux_t *p);
extern void drawWorld();
extern void eventWorld();
extern void startArena();
extern void stopArena();
extern void quitWorld();

#endif
