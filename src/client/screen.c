
#include <stdlib.h>
#include <assert.h>

#include "main.h"
#include "list.h"

#include "interface.h"
#include "screen.h"
#include "layer.h"
#include "myTimer.h"

//#define DEBUG_TIME_DRAW

static screen_t *currentScreen;
static screen_t *futureScreen;

static list_t *listScreen;

static bool_t isScreenInit = FALSE;

bool_t screen_is_inicialized()
{
	return isScreenInit;
}

screen_t *screen_new(char *name, void (*fce_start) (), void (*fce_event) (),
				void (*fce_draw) (), void (*fce_stop) ())
{
	screen_t *new;

	assert(name != NULL);
	assert(fce_start != NULL);
	assert(fce_event != NULL);
	assert(fce_draw != NULL);
	assert(fce_stop != NULL);

	new = malloc(sizeof(screen_t));
	new->name = strdup(name);
	new->fce_start = fce_start;
	new->fce_event = fce_event;
	new->fce_draw = fce_draw;
	new->fce_stop = fce_stop;

	return new;
}

void screen_destroy(screen_t * p)
{
	assert(p != NULL);

	free(p->name);
	free(p);
}

void screen_register(screen_t * p)
{
	assert(p != NULL);

	DEBUG_MSG(_("Registering screen: \"%s\"\n"), p->name);

	list_add(listScreen, p);
}

void screen_init()
{
	listScreen = list_new();

	currentScreen = NULL;
	futureScreen = NULL;

	isScreenInit = TRUE;
}

static screen_t *findScreen(char *name)
{
	screen_t *this;
	int i;

	for (i = 0; i < listScreen->count; i++) {
		this = (screen_t *) (listScreen->list[i]);
		assert(this != NULL);

		if (strcmp(name, this->name) == 0) {
			return this;
		}
	}

	return NULL;
}

void screen_set(char *name)
{
	futureScreen = findScreen(name);
	assert(futureScreen != NULL);
}

void screen_switch()
{
	if (futureScreen == NULL) {
		return;
	}

	layer_flush();

	if (currentScreen != NULL) {
		DEBUG_MSG(_("Stopping screen: \"%s\"\n"), currentScreen->name);
		currentScreen->fce_stop();
	}

	currentScreen = futureScreen;
	futureScreen = NULL;

	//printf("switch screen %s..\n", currentScreen->name);

	DEBUG_MSG(_("Starting screen: \"%s\"\n"), currentScreen->name);

	currentScreen->fce_start();
}

void screen_start(char *name)
{
	screen_set(name);
	screen_switch();
}

char *screen_get()
{
	assert(currentScreen != NULL);
	return currentScreen->name;
}

void screen_draw()
{
	static int count = 0;

	count++;

	if (count == 1) {
		count = 0;

		assert(currentScreen != NULL);

#ifdef DEBUG_TIME_DRAW
		my_time_t prev;

		prev = timer_get_current_timeMicro();
#endif

		currentScreen->fce_draw();

		interface_refresh();

#ifdef DEBUG_TIME_DRAW
		printf("c draw time = %d\n", timer_get_current_timeMicro() - prev);
#endif
	}
}


void screen_event()
{
	assert(currentScreen != NULL);
#if 0
	static my_time_t last = 0;

	if( last == 0 )
	{
  		last = timer_get_current_time();
	}

	printf("%d\n", timer_get_current_time()-last);
	last = timer_get_current_time();
#endif
	currentScreen->fce_event();

#ifdef DEBUG_TIME_EVENT
	printf("c event time = %d\n", timer_get_current_timeMicro() - prev);
#endif
}

void screen_quit()
{
	assert(listScreen != NULL);

	list_destroy_item(listScreen, screen_destroy);
	isScreenInit = FALSE;
}
