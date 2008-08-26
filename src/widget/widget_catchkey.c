
#include <stdlib.h>
#include <assert.h>

#include "main.h"
#include "interface.h"
#include "font.h"

#include "widget.h"
#include "widget_catchkey.h"

widget_t* newWidgetCatchkey(int key, int x, int y, void *event)
{
	widget_catchkey_t *new;

	new = malloc( sizeof(widget_catchkey_t) );
	new->key = key;
	new->fce_event = event;
	new->active = FALSE;

	return newWidget(WIDGET_TYPE_CATCHKEY, x, y, WIDGET_CATCHKEY_WIDTH, WIDGET_CATCHKEY_HEIGHT, new);
}

int getWidgetCatchKey(widget_t *widget)
{
	widget_catchkey_t *p;

	assert( widget != NULL );
	assert( widget->type == WIDGET_TYPE_CATCHKEY );

	p = (widget_catchkey_t *) widget->private_data;

	return p->key;
}

void setWidgetCatchKey(widget_t *widget, int key)
{
	widget_catchkey_t *p;

	assert( widget != NULL );
	assert( widget->type == WIDGET_TYPE_CATCHKEY );

	p = (widget_catchkey_t *) widget->private_data;

	p->key = key;
}

void drawWidgetCatchkey(widget_t *widget)
{
	widget_catchkey_t *p;
	char *name;

	assert( widget != NULL );
	assert( widget->type == WIDGET_TYPE_CATCHKEY );

	p = (widget_catchkey_t *) widget->private_data;
	name = NULL;

	if( p->key != WIDGET_CATCHKEY_NOKEY )
	{
		name = (char *) SDL_GetKeyName(p->key);
	}

	if( name == NULL )
	{
		name = "no key";
	}

	if( p->active )
	{
		drawFont(name, widget->x, widget->y, COLOR_RED);
	}
	else
	{
		drawFont(name, widget->x, widget->y, COLOR_WHITE);
	}
}

static int getPessAnyKey()
{
	Uint8 *mapa;
	int i;

	mapa = SDL_GetKeyState(NULL);

	for( i = SDLK_FIRST ; i <= SDLK_COMPOSE ; i++ )
	{
		if( i == SDLK_NUMLOCK || // black key
		    i == SDLK_CAPSLOCK ||
		    i == SDLK_SCROLLOCK ) 
		{
			continue;
		}

		if( mapa[i] == SDL_PRESSED )
		{
			return i;
		}
	}

	return WIDGET_CATCHKEY_NOKEY;
}

void eventWidgetCatchkey(widget_t *widget)
{
	widget_catchkey_t *p;
	int x, y;

	assert( widget != NULL );
	assert( widget->type == WIDGET_TYPE_CATCHKEY );

	p = (widget_catchkey_t *) widget->private_data;
	
	getMousePosition(&x, &y);

	if( isMouseClicked() )
	{
		if( x >= widget->x && x <= widget->x+WIDGET_CATCHKEY_WIDTH && 
		    y >= widget->y && y <= widget->y+WIDGET_CATCHKEY_HEIGHT )
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
			p->fce_event(widget);
		}
	}
}

void destroyWidgetCatchkey(widget_t *widget)
{
	widget_catchkey_t *p;

	assert( widget != NULL );
	assert( widget->type == WIDGET_TYPE_CATCHKEY );

	p = (widget_catchkey_t *) widget->private_data;

	free(p);
	destroyWidget(widget);
}
