
#ifndef MY_TIME_H
#    define MY_TIME_H
#    define my_time_t 		unsigned int
#    define TIMER_ONE		0
#    define TIMER_PERIODIC	1

#    include "main.h"
#    include "list.h"

typedef struct my_timer_struct {
	unsigned int id;
	void (*fce) (void *p);
	void *arg;
	int type;
	my_time_t createTime;
	my_time_t time;
} my_timer_t;

extern list_t *timer_new();
extern void timer_restart();
extern my_time_t timer_get_current_time();
extern int timer_add_task(list_t * listTimer, int type, void (*fce) (void *p),
						  void *arg, my_time_t my_time);
extern void timer_event(list_t * listTimer);
extern void timer_del(list_t * listTimer, int id);
extern void timer_destroy(list_t * listTimer);
#endif
