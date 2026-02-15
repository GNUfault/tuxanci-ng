#include <stdlib.h>
#include <assert.h>
#include <SDL2/SDL.h>

#include "main.h"
#include "interface.h"
#include "font.h"
#include "mouse_buffer.h"

#include "widget.h"
#include "widget_catchkey.h"

widget_t *catch_key_new(int key, int x, int y, void *event)
{
	widget_catchkey_t *new;

	new = malloc(sizeof(widget_catchkey_t));
	new->key = key;
	new->fce_event = event;
	new->active = FALSE;

	return widget_new(WIDGET_TYPE_CATCHKEY, x, y, WIDGET_CATCHKEY_WIDTH,
			  WIDGET_CATCHKEY_HEIGHT, new);
}

int catch_key_get(widget_t *widget)
{
	widget_catchkey_t *p;

	assert(widget != NULL);
	assert(widget->type == WIDGET_TYPE_CATCHKEY);

	p = (widget_catchkey_t *) widget->private_data;

	return p->key;
}

void catch_key_set(widget_t *widget, int key)
{
	widget_catchkey_t *p;

	assert(widget != NULL);
	assert(widget->type == WIDGET_TYPE_CATCHKEY);

	p = (widget_catchkey_t *) widget->private_data;

	p->key = key;
}

void catch_key_draw(widget_t *widget)
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

static int getPressAnyKey()
{
	int numkeys = 0;
	const Uint8 *state = SDL_GetKeyboardState(&numkeys);
	int sc;

	for (sc = 0; sc < numkeys; sc++) {
		if (!state[sc]) continue;

		SDL_Keycode keycode = SDL_GetKeyFromScancode((SDL_Scancode)sc);
		if (keycode == SDLK_NUMLOCKCLEAR ||
			keycode == SDLK_CAPSLOCK ||
			keycode == SDLK_SCROLLLOCK) {
			continue;
		}

		return (int) keycode;
	}

	return WIDGET_CATCHKEY_NOKEY;
}

void catch_key_event(widget_t *widget)
{
	widget_catchkey_t *p;

	assert(widget != NULL);
	assert(widget->type == WIDGET_TYPE_CATCHKEY);

	p = (widget_catchkey_t *) widget->private_data;

	if (mouse_buffer_is_on_area(0, 0, 0, 0, MOUSE_BUF_AREA_NONE|MOUSE_BUF_CLICK)) {
		if (mouse_buffer_is_on_area(widget->x, widget->y,
					    WIDGET_CATCHKEY_WIDTH,
					    WIDGET_CATCHKEY_HEIGHT,
					    MOUSE_BUF_CLICK)) {
			p->active = TRUE;
		} else {
			p->active = FALSE;
		}
	}

	if (p->active == TRUE) {
		int key;

		key = getPressAnyKey();

		if (key != WIDGET_CATCHKEY_NOKEY) {
			p->key = key;
			p->fce_event(widget);
			p->active = FALSE;
		}
	}
}

void catch_key_destroy(widget_t *widget)
{
	widget_catchkey_t *p;

	assert(widget != NULL);
	assert(widget->type == WIDGET_TYPE_CATCHKEY);

	p = (widget_catchkey_t *) widget->private_data;

	free(p);
	widget_destroy(widget);
}
