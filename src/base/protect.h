
#ifndef PROTECT_H

#define PROTECT_H

#include "base/main.h"
#include "base/myTimer.h"

#define PROTECT_PING_INTERVAL_TIMEOUT	5000
#define PROTECT_SPEED_INTERVAL_TIMEOUT	40
#define PROTECT_SPEED_AVARAGE	10

typedef struct protect_struct
{
	my_time_t lastPing;

	my_time_t lastMove;
	my_time_t avarage;
	int count;

	bool_t isDown;
} protect_t;

extern protect_t* newProtect();
extern void refreshLastMove(protect_t *p);
extern void rereshLastPing(protect_t *p);
extern bool_t isDown(protect_t *p);
extern void destroyProtect(protect_t *p);

#endif
