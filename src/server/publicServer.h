
#ifndef PUBLIC_SERVER_H

#define PUBLIC_SERVER_H

#include "base/arena.h"

#define WORLD_COUNT_ROUND_UNLIMITED	-1

extern void countRoundInc();
extern int initPublicServer();
extern int getChoiceArenaId();
extern void eventPublicServer();
extern void my_handler_quit(int n);
extern void quitPublicServer();
extern int startPublicServer();

#endif
