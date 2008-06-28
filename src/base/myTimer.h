
#ifndef MY_TIME_H

#define MY_TIME_H

#include "base/main.h"
#include "base/list.h"

#define my_time_t unsigned int

#define TIMER_ONE	0
#define TIMER_PERIODIC	1

typedef struct my_timer_struct
{
	unsigned int id;
	void (*fce)(void *p);
	void *arg;
	int type;
	my_time_t createTime;
	my_time_t time;
} my_timer_t;

extern list_t* newTimer();
extern void restartTimer();
extern my_time_t getMyTime();
extern int addTaskToTimer(list_t *listTimer, int type, void (*fce)(void *p), void *arg, my_time_t my_time);
extern void eventTimer(list_t *listTimer);
extern void delTimer(list_t *listTimer, int id);
extern void destroyTimer(list_t *listTimer);

#endif
