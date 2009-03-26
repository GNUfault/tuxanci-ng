
#include <time.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <signal.h>
#ifdef __WIN32__
#    include <time.h>
#endif

#include "main.h"
#include "list.h"
#include "myTimer.h"
#ifndef PUBLIC_SERVER
#    include "interface.h"
#endif

static struct timeval my_start = {.tv_sec = 0,.tv_usec = 0 };

list_t *newTimer()
{
	return newList();
}

void restartTimer()
{
	gettimeofday(&my_start, NULL);
}


my_time_t getMyTime()
{
	struct timeval now;
	my_time_t ticks;

	if (my_start.tv_sec == 0 && my_start.tv_usec == 0)
		restartTimer();

	gettimeofday(&now, NULL);
	ticks = (now.tv_sec - my_start.tv_sec) * 1000 + (now.tv_usec - my_start.tv_usec) / 1000;
	//printf("-> %d\n", ticks);

	return ticks;
}

my_time_t getMyTimeMicro()
{
	static struct timeval my_micro_start = {.tv_sec = 0,.tv_usec = 0 };
	struct timeval now;
	my_time_t ticks;

	if (my_micro_start.tv_sec == 0 && my_micro_start.tv_usec == 0)
		gettimeofday(&my_micro_start, NULL);

	gettimeofday(&now, NULL);
	ticks = (now.tv_sec - my_micro_start.tv_sec) * 1000 * 1000 + (now.tv_usec - my_micro_start.tv_usec);
	//printf("-> %d\n", ticks);

	return ticks;
}

my_timer_t *newTimerItem(int type, void (*fce) (void *p), void *arg,
						my_time_t my_time)
{
	static int new_id = 0;
	my_timer_t *new;

	assert(fce != NULL);

	new = malloc(sizeof(my_timer_t));
	memset(new, 0, sizeof(my_timer_t));
	new->id = new_id++;
	new->type = type;
	new->fce = fce;
	new->arg = arg;
	new->createTime = getMyTime();
	new->time = my_time;

	return new;
}

static void destroyTimerItem(my_timer_t * p)
{
	assert(p != NULL);
	free(p);
}

int addTaskToTimer(list_t * listTimer, int type, void (*fce) (void *p), void *arg, my_time_t my_time)
{
	my_timer_t *new;

	new = newTimerItem(type, fce, arg, my_time);
	addList(listTimer, new);

	return new->id;
}

void eventTimer(list_t * listTimer)
{
	int i;
	my_timer_t *thisTimer;
	my_time_t currentTime;

	currentTime = getMyTime();

	for (i = 0; i < listTimer->count; i++) {
		thisTimer = (my_timer_t *) listTimer->list[i];
		assert(thisTimer != NULL);

		switch (thisTimer->type) {
			case TIMER_ONE:
				if (currentTime >= thisTimer->createTime + thisTimer->time) {
					thisTimer->fce(thisTimer->arg);
					delListItem(listTimer, i, free);
					i--;
				}
				break;
			case TIMER_PERIODIC:
				if (currentTime >= thisTimer->createTime + thisTimer->time) {
					thisTimer->fce(thisTimer->arg);
					thisTimer->createTime = getMyTime();
				}
				break;
			default:
				assert(!_("Timer is really weirdly set!"));
				break;
		}
	}
}

void delTimer(list_t * listTimer, int id)
{
	my_timer_t *thisTimer;
	int i;

	for (i = 0; i < listTimer->count; i++) {
		thisTimer = (my_timer_t *) listTimer->list[i];

		assert(thisTimer != NULL);

		if (thisTimer->id == (unsigned)id) {
			delListItem(listTimer, i, free);
			return;
		}
	}
	assert(!_("There is no such ID!"));
}


void destroyTimer(list_t * listTimer)
{
	destroyListItem(listTimer, destroyTimerItem);
}
