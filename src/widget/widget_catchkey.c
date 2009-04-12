
#include <stdlib.h>
#include <assert.h>

#include "main.h"
#include "interface.h"
#include "font.h"

#include "widget.h"
#include "widget_catchkey.h"

widget_t *catchKey_new(int key, int x, int y, void *event)
{
	widget_catchkey_t *new;

	new = malloc(sizeof(widget_catchkey_t));
	new->key = key;
	new->fce_event = event;
	new->active = FALSE;

	return widget_new(WIDGET_TYPE_CATCHKEY, x, y, WIDGET_CATCHKEY_WIDTH, WIDGET_CATCHKEY_HEIGHT, new);
}

int catchKey_get(widget_t * widget)
{
	widget_catchkey_t *p;

	assert(widget != NULL);
	assert(widget->type == WIDGET_TYPE_CATCHKEY);

	p = (widget_catchkey_t *) widget->private_data;

	return p->key;
}

void catchKey_set(widget_t * widget, int key)
{
	widget_catchkey_t *p;

	assert(widget != NULL);
	assert(widget->type == WIDGET_TYPE_CATCHKEY);

	p = (widget_catchkey_t *) widget->private_data;

	p->key = key;
}

void catchKey_draw(widget_t * widget)
{
	widget_catchkey_t *p;
	char *name;

	assert(widget != NULL);
	assert(widget->type == WIDGET_TYPE_CATCHKEY);

	p = (widget_catchkey_t *) widget->private_data;
	name = NULL;

	if (p->key != WIDGET_CATCHKEY_NOKEY) {
		name = (char *) SDL_GetKeyName(p->key);
	}

	if (name == NULL) {
		name = "no key";
	}

	if (p->active) {
		font_draw(name, widget->x, widget->y, COLOR_RED);
	} else {
		font_draw(name, widget->x, widget->y, COLOR_WHITE);
	}
}

static int getPessAnyKey()
{
	Uint8 *mapa;
	int i;

	mapa = SDL_GetKeyState(NULL);

	for (i = SDLK_FIRST; i <= SDLK_COMPOSE; i++) {
		if (i == SDLK_NUMLOCK ||	// black key
		    i == SDLK_CAPSLOCK ||
		    i == SDLK_SCROLLOCK) {
			continue;
		}

		if (mapa[i] == SDL_PRESSED) {
			return i;
		}
	}

	return WIDGET_CATCHKEY_NOKEY;
}

void catchKey_event(widget_t * widget)
{
	widget_catchkey_t *p;
	int x, y;

	assert(widget != NULL);
	assert(widget->type == WIDGET_TYPE_CATCHKEY);

	p = (widget_catchkey_t *) widget->private_data;

	interface_get_mouse_position(&x, &y);

	if (interface_is_mouse_clicket()) {
		if (x >= widget->x && x <= widget->x + WIDGET_CATCHKEY_WIDTH &&
		    y >= widget->y && y <= widget->y + WIDGET_CATCHKEY_HEIGHT) {
			p->active = TRUE;
		} else {
			p->active = FALSE;
		}
	}

	if (p->active == TRUE) {
		int key;

		key = getPessAnyKey();

		if (key != WIDGET_CATCHKEY_NOKEY) {
			p->key = key;
			p->fce_event(widget);
		}
	}
}

void catchKey_destroy(widget_t * widget)
{
	widget_catchkey_t *p;

	assert(widget != NULL);
	assert(widget->type == WIDGET_TYPE_CATCHKEY);

	p = (widget_catchkey_t *) widget->private_data;

	free(p);
	widget_destroy(widget);
}
