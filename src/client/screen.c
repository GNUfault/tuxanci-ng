
#include <stdlib.h>
#include <assert.h>

#include "base/main.h"
#include "base/list.h"

#include "client/interface.h"
#include "client/screen.h"
#include "client/layer.h"

static screen_t *currentScreen;
static list_t *listScreen;

static bool_t isScreenInit = FALSE;

bool_t isScreenInicialized()
{
	return isScreenInit;
}

screen_t* newScreen(char *name,
		void (*fce_start)(), void (*fce_event)(),
		void (*fce_draw)(), void (*fce_stop)())
{
	screen_t *new;

	assert( name != NULL );
	assert( fce_start != NULL );
	assert( fce_event != NULL );
	assert( fce_draw != NULL );
	assert( fce_stop != NULL );

	new = malloc( sizeof(screen_t) );
	new->name = strdup(name);
	new->fce_start = fce_start;
	new->fce_event = fce_event;
	new->fce_draw = fce_draw;
	new->fce_stop = fce_stop;

	return new;
}

void destroyScreen(screen_t *p)
{
	assert( p != NULL );

	free(p->name);
	free(p);
}

void registerScreen(screen_t *p)
{
	assert( p != NULL );

	printf("register screen %s..\n", p->name);
	addList(listScreen, p);
}

void initScreen()
{
	listScreen = newList();
	currentScreen = NULL;
	isScreenInit = TRUE;
}

void setScreen(char *name)
{
	int i;
	screen_t *this;

	assert( name != NULL );

	if( currentScreen != NULL &&
	    strcmp(currentScreen->name, name) == 0 )
	{
		return;
	}

	printf("switch screen %s..\n", name);

	for( i = 0 ; i < listScreen->count ; i++ )
	{
		this = (screen_t *) (listScreen->list[i]);
		assert( this != NULL );

		if( strcmp(name, this->name) == 0 )
		{
			flushLayer();

			if( currentScreen != NULL )
			{
				printf("stop screen %s..\n", currentScreen->name);
				currentScreen->fce_stop();
			}

			currentScreen = this;
			printf("start screen %s..\n", currentScreen->name);
			currentScreen->fce_start();

			return;
		}
	}

	assert( ! "screen sa nenasiel !" );
}

char* getScreen()
{
	assert( currentScreen != NULL );
	return currentScreen->name;
}

void drawScreen()
{
	assert( currentScreen != NULL );

	currentScreen->fce_draw();

	interfaceRefresh();
}

void eventScreen()
{
	assert( currentScreen != NULL );

	currentScreen->fce_event();
}

void quitScreen()
{
	assert( listScreen != NULL );

	destroyListItem(listScreen, destroyScreen);
	isScreenInit = FALSE;
}

