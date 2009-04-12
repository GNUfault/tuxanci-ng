
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

list_t *timer_new()
{
	return list_new();
}

void timer_restart()
{
	gettimeofday(&my_start, NULL);
}


my_time_t timer_get_current_time()
{
	struct timeval now;
	my_time_t ticks;

	if (my_start.tv_sec == 0 && my_start.tv_usec == 0)
		timer_restart();

	gettimeofday(&now, NULL);
	ticks = (now.tv_sec - my_start.tv_sec) * 1000 + (now.tv_usec - my_start.tv_usec) / 1000;
	//printf("-> %d\n", ticks);

	return ticks;
}

my_time_t timer_get_current_timeMicro()
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

my_timer_t *timer_newItem(int type, void (*fce) (void *p), void *arg,
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
	new->createTime = timer_get_current_time();
	new->time = my_time;

	return new;
}

static void timer_destroyItem(my_timer_t * p)
{
	assert(p != NULL);
	free(p);
}

int timer_add_task(list_t * listTimer, int type, void (*fce) (void *p), void *arg, my_time_t my_time)
{
	my_timer_t *new;

	new = timer_newItem(type, fce, arg, my_time);
	list_add(listTimer, new);

	return new->id;
}

void timer_event(list_t * listTimer)
{
	int i;
	my_timer_t *thisTimer;
	my_time_t currentTime;

	currentTime = timer_get_current_time();

	for (i = 0; i < listTimer->count; i++) {
		thisTimer = (my_timer_t *) listTimer->list[i];
		assert(thisTimer != NULL);

		switch (thisTimer->type) {
			case TIMER_ONE:
				if (currentTime >= thisTimer->createTime + thisTimer->time) {
					thisTimer->fce(thisTimer->arg);
					list_del_item(listTimer, i, free);
					i--;
				}
				break;
			case TIMER_PERIODIC:
				if (currentTime >= thisTimer->createTime + thisTimer->time) {
					thisTimer->fce(thisTimer->arg);
					thisTimer->createTime = timer_get_current_time();
				}
				break;
			default:
				assert(!_("Timer is really weirdly set!"));
				break;
		}
	}
}

void timer_del(list_t * listTimer, int id)
{
	my_timer_t *thisTimer;
	int i;

	for (i = 0; i < listTimer->count; i++) {
		thisTimer = (my_timer_t *) listTimer->list[i];

		assert(thisTimer != NULL);

		if (thisTimer->id == (unsigned)id) {
			list_del_item(listTimer, i, free);
			return;
		}
	}
	assert(!_("There is no such ID!"));
}


void timer_destroy(list_t * listTimer)
{
	list_destroy_item(listTimer, timer_destroyItem);
}
