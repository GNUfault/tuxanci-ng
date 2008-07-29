
#include <stdlib.h>

#include "main.h"
#include "interface.h"
#include "font.h"
#include "widget_catchkey.h"

widget_catchkey_t* newWidgetCatchkey(int key, int x, int y, void *event)
{
	widget_catchkey_t *new;

	new = malloc( sizeof(widget_catchkey_t) );

	new->x = x;
	new->y = y;
	new->key = key;
	new->fce_event = event;

	return new;
}

int getWidgetCatchKey(widget_catchkey_t *p)
{
	return p->key;
}

void setWidgetCatchKey(widget_catchkey_t *p, int key)
{
	p->key = key;
}

void drawWidgetCatchkey(widget_catchkey_t *p)
{
	char *name;

	name = NULL;

	if( p->key != WIDGET_CATCHKEY_NOKEY )
	{
		name = (char *) SDL_GetKeyName(p->key);
	}

	if( name == NULL )
	{
		name = "no key";
	}

	drawFont(name, p->x, p->y, COLOR_WHITE);
}

static int getPessAnyKey()
{
	Uint8 *mapa;
	int i;

	mapa = SDL_GetKeyState(NULL);

	for( i = SDLK_FIRST ; i <= SDLK_F15 ; i++ )
	{
		if( mapa[i] == SDL_PRESSED )
		{
			return i;
		}
	}

	return WIDGET_CATCHKEY_NOKEY;
}

void eventWidgetCatchkey(widget_catchkey_t *p)
{
	int x, y;

	getMousePosition(&x, &y);

	if( isMouseClicked() )
	{
		if( x >= p->x && x <= p->x+WIDGET_CATCHKEY_WIDTH && 
		    y >= p->y && y <= p->y+WIDGET_CATCHKEY_HEIGHT )
		{
			p->active = TRUE;
		}
		else
		{
			p->active = FALSE;
		}
	}

	if( p->active == TRUE )
	{
		int key;

		key = getPessAnyKey();

		if( key != WIDGET_CATCHKEY_NOKEY )
		{
			p->key = key;
			p->fce_event(p);
		}
	}
}

void destroyWidgetCatchkey(widget_catchkey_t *p)
{
	free(p);
}
