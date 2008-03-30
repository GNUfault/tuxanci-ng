
#ifndef MY_TIME_H

#define MY_TIME_H

#include "main.h"

#define my_time_t unsigned int

typedef struct my_timer_struct
{
	unsigned int id;
	void (*fce)(void *p);
	void *arg;
	my_time_t time;
} my_timer_t;

extern list_t* newTimer();
extern my_time_t getMyTime();
extern int addTimer(list_t *listTimer, void (*fce)(void *p), void *arg, my_time_t my_time);
extern void eventTimer(list_t *listTimer);
extern void delTimer(list_t *listTimer, int id);
extern void destroyTimer(list_t *listTimer);

#endif
