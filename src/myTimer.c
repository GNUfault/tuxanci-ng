
#include <time.h>
#include <assert.h>
#include <stdlib.h>
#include <sys/time.h>
#include <signal.h>

#include "main.h"
#include "list.h"
#include "myTimer.h"

#ifndef PUBLIC_SERVER
#include "interface.h"
#endif


list_t* newTimer()
{
	return newList();
}

my_time_t getMyTime()
{
	static struct timeval start = { .tv_sec = 0, .tv_usec = 0 };
	struct timeval now;
	my_time_t ticks;

	if(  start.tv_sec == 0 && start.tv_usec == 0 )
	{
		gettimeofday(&start, NULL);
	}

	gettimeofday(&now, NULL);

	ticks = (now.tv_sec-start.tv_sec)*1000+(now.tv_usec-start.tv_usec)/1000;

	return ticks;
}

int addTimer(list_t *listTimer, void (*fce)(void *p), void *arg, my_time_t my_time)
{
	static int new_id = 0;
	my_timer_t *new;
	my_time_t currentTime;

 	currentTime = getMyTime();

	new = malloc( sizeof(my_timer_t) );
	assert( new != NULL );

	new->id = new_id++;
	new->fce = fce;
	new->arg = arg;
	new->time = currentTime + my_time;

	addList(listTimer, new);

	return new->id;
}

void eventTimer(list_t *listTimer)
{
	int i;
	my_timer_t *thisTimer;
	my_time_t currentTime;

 	currentTime = getMyTime();

	for( i = 0 ; i < listTimer->count ; i++ )
	{
		thisTimer = (my_timer_t *)listTimer->list[i];
		assert( thisTimer != NULL );

		if( currentTime >= thisTimer->time )
		{
			thisTimer->fce(thisTimer->arg);

			delListItem(listTimer, i, free);
			i--;
		}
	}
}

void delTimer(list_t *listTimer, int id)
{
	int i;
	my_timer_t *thisTimer;

	for( i = 0 ; i < listTimer->count ; i++ )
	{
		thisTimer = (my_timer_t *)listTimer->list[i];
		assert( thisTimer != NULL );

		if( thisTimer->id == id )
		{
			delListItem(listTimer, i, free);
			return;
		}
	}

	assert( ! "Uloha s id nenajdena !" );
}


void destroyTimer(list_t *listTimer)
{
	destroyListItem(listTimer, free);
}
