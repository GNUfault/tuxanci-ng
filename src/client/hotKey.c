#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "main.h"
#include "list.h"
#include "myTimer.h"

#include "interface.h"
#include "hotKey.h"

typedef struct hotKey_struct {
	SDL_Keycode key;
	bool_t active;
	void (*handler) ();
} hotKey_t;

static list_t *listHotKey;
static my_time_t lastActive;

static hotKey_t *newHotKey(SDL_Keycode key, void (*handler) ())
{
	hotKey_t *new;

	new = malloc(sizeof(hotKey_t));
	new->key = key;
	new->active = TRUE;
	new->handler = handler;

	return new;
}

static void destroyHotKey(hotKey_t *hotkey)
{
	free(hotkey);
}

static void destroyHotKey_void(void *p)
{
        destroyHotKey((hotKey_t *) p);
}

static hotKey_t *findHotkey(SDL_Keycode key)
{
	int i;

	for (i = 0; i < listHotKey->count; i++) {
		hotKey_t *this;

		this = (hotKey_t *) listHotKey->list[i];

		if (this->key == key) {
			return this;
		}
	}

	return NULL;
}

void hot_key_init()
{
	listHotKey = list_new();
	lastActive = timer_get_current_time();
}

void hot_key_register(SDL_Keycode key, void (*handler) ())
{
	hotKey_t *hotkey;
/*
	if (findHotkey(key) != NULL) {
		assert(!"Conflict with register key");
	}
*/
	hotkey = newHotKey(key, handler);
	list_ins(listHotKey, 0, hotkey);
}

void hot_key_unregister(SDL_Keycode key)
{
	hotKey_t *hotkey;
	int my_index;

	hotkey = findHotkey(key);

	if (hotkey == NULL) {
		fatal("Unregistering not registered hotkey");
	}

	my_index = list_search(listHotKey, hotkey);
	assert(my_index != -1);
	list_del_item(listHotKey, my_index, destroyHotKey_void);
}

void hot_key_enable(SDL_Keycode key)
{
	hotKey_t *hotkey;

	hotkey = findHotkey(key);

	if (hotkey == NULL) {
		fatal("Enabling not registered hotkey");
	}

	lastActive = timer_get_current_time();
	hotkey->active = TRUE;
}

void hot_key_disable(SDL_Keycode key)
{
	hotKey_t *hotkey;

	hotkey = findHotkey(key);

	if (hotkey == NULL) {
		fatal("Disabling not registered hotkey");
	}

	lastActive = timer_get_current_time();
	hotkey->active = FALSE;
}

void hot_key_event()
{
	my_time_t currentTime;
	const Uint8 *state;
	int i;
	int numkeys = 0;

	currentTime = timer_get_current_time();

	if (currentTime - lastActive < HOTKEY_ACTIVE_INTERVAL) {
		return;
	}

	state = SDL_GetKeyboardState(&numkeys);

	for (i = 0; i < listHotKey->count; i++) {
		hotKey_t *this;

		this = (hotKey_t *) listHotKey->list[i];

		SDL_Scancode sc = SDL_GetScancodeFromKey(this->key);
		if ((int)sc < numkeys && state[(int)sc] && this->active == TRUE) {
			lastActive = timer_get_current_time();
			/*printf("hotKey = %d\n", this->key);*/
			this->handler();

			return;
		}
	}
}

void hot_key_quit()
{
	list_destroy_item(listHotKey, destroyHotKey_void);
}
