
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "base/main.h"
#include "base/myTimer.h"
#include "base/protect.h"

protect_t* newProtect()
{
	protect_t *new;

	new = malloc( sizeof(protect_t) );
	new->lastMove = getMyTime();
	new->lastPing = getMyTime();
	new->avarage = 0;
	new->count = 0;
	new->isDown = FALSE;
	return new;
}

void refreshLastMove(protect_t *p)
{
	my_time_t currentTime;
	my_time_t interval;

	currentTime = getMyTime();

	interval = currentTime - p->lastMove;
	p->lastMove = getMyTime();

	p->avarage += interval;
	p->count++;

	if( p->count == PROTECT_SPEED_AVARAGE )
	{
		int index;

		index = p->avarage/PROTECT_SPEED_AVARAGE;
		p->avarage = 0;
		p->count = 0;

#if 0
		if( index < PROTECT_SPEED_INTERVAL_TIMEOUT )
		{
			p->isDown = TRUE;
		}
#endif
		//printf("speed index = %d\n", index);
	}
}

void rereshLastPing(protect_t *p)
{
	//my_time_t currentTime;
	//my_time_t interval;
	
	//currentTime = getMyTime();

	//interval = currentTime - p->lastPing;
	p->lastPing = getMyTime();

	//printf("interval = %d\n", interval);
}

bool_t isDown(protect_t *p)
{
	my_time_t currentTime;
	my_time_t interval;

	currentTime = getMyTime();

	interval = currentTime - p->lastPing;

	if( interval > PROTECT_PING_INTERVAL_TIMEOUT )
	{
		p->isDown = TRUE;
	}

	return p->isDown;
}

void destroyProtect(protect_t *p)
{
	assert( p != NULL );
	free(p);
}
