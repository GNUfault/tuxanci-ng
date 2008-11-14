
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

bool_t isScreenInicialized()
{
	return isScreenInit;
}

screen_t *newScreen(char *name,
					void (*fce_start) (), void (*fce_event) (),
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

void destroyScreen(screen_t * p)
{
	assert(p != NULL);

	free(p->name);
	free(p);
}

void registerScreen(screen_t * p)
{
	assert(p != NULL);

	DEBUG_MSG(_("Registering screen: \"%s\"\n"), p->name);

	addList(listScreen, p);
}

void initScreen()
{
	listScreen = newList();

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

void setScreen(char *name)
{
	futureScreen = findScreen(name);
	assert(futureScreen != NULL);
}

void switchScreen()
{
	if (futureScreen == NULL) {
		return;
	}

	flushLayer();

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

void startScreen(char *name)
{
	setScreen(name);
	switchScreen();
}

char *getScreen()
{
	assert(currentScreen != NULL);
	return currentScreen->name;
}

void drawScreen()
{
	static int count = 0;

	count++;

	if (count == 1) {
		count = 0;

		assert(currentScreen != NULL);

#ifdef DEBUG_TIME_DRAW
		my_time_t prev;

		prev = getMyTimeMicro();
#endif

		currentScreen->fce_draw();

		interfaceRefresh();

#ifdef DEBUG_TIME_DRAW
		printf("c draw time = %d\n", getMyTimeMicro() - prev);
#endif
	}
}


void eventScreen()
{
	assert(currentScreen != NULL);
#if 0
	static my_time_t last = 0;

	if( last == 0 )
	{
  		last = getMyTime();
	}

	printf("%d\n", getMyTime()-last);
	last = getMyTime();
#endif
	currentScreen->fce_event();

#ifdef DEBUG_TIME_EVENT
	printf("c event time = %d\n", getMyTimeMicro() - prev);
#endif
}

void quitScreen()
{
	assert(listScreen != NULL);

	destroyListItem(listScreen, destroyScreen);
	isScreenInit = FALSE;
}
