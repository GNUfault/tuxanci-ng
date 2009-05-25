#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "main.h"
#include "myTimer.h"
#include "protect.h"

protect_t *newProtect()
{
	protect_t *new;

	new = malloc(sizeof(protect_t));
	new->lastMove = timer_get_current_time();
	new->lastPing = timer_get_current_time();
	new->avarage = 0;
	new->count = 0;
	new->isDown = FALSE;

	return new;
}

void refreshLastMove(protect_t *p)
{
	my_time_t currentTime;
	my_time_t interval;

	currentTime = timer_get_current_time();
	interval = currentTime - p->lastMove;

	p->lastMove = timer_get_current_time();
	p->avarage += interval;
	p->count++;

	if (p->count == PROTECT_SPEED_AVARAGE) {
		int my_index;

		my_index = p->avarage / PROTECT_SPEED_AVARAGE;
		p->avarage = 0;
		p->count = 0;

/*
		if (my_index < PROTECT_SPEED_INTERVAL_TIMEOUT) {
			p->isDown = TRUE;
		}
*/

		/*printf("speed index = %d\n", my_index);*/
	}
}

void rereshLastPing(protect_t *p)
{
	/*
	my_time_t currentTime;
	my_time_t interval;
	currentTime = timer_get_current_time();
	interval = currentTime - p->lastPing;
	*/

	p->lastPing = timer_get_current_time();

	/*printf("interval = %d\n", interval);*/
}

bool_t isDown(protect_t *p)
{
	my_time_t currentTime;
	my_time_t interval;

	currentTime = timer_get_current_time();
	interval = currentTime - p->lastPing;

	if (interval > PROTECT_PING_INTERVAL_TIMEOUT) {
		p->isDown = TRUE;
	}

	return p->isDown;
}

void destroyProtect(protect_t *p)
{
	assert(p != NULL);
	free(p);
}
