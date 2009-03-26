
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "main.h"
#include "list.h"
#include "myTimer.h"

#include "interface.h"
#include "hotKey.h"

typedef struct hotKey_struct {
	SDLKey key;
	bool_t active;
	void (*handler) ();
} hotKey_t;

static list_t *listHotKey;
static my_time_t lastActive;

static hotKey_t *newHotKey(SDLKey key, void (*handler) ())
{
	hotKey_t *new;

	new = malloc(sizeof(hotKey_t));
	new->key = key;
	new->active = TRUE;
	new->handler = handler;

	return new;
}

static void destroyHotKey(hotKey_t * hotkey)
{
	free(hotkey);
}

static hotKey_t *findHotkey(SDLKey key)
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

void initHotKey()
{
	listHotKey = newList();
	lastActive = getMyTime();
}

void registerHotKey(SDLKey key, void (*handler) ())
{
	hotKey_t *hotkey;
/*
	if (findHotkey(key) != NULL) {
		assert(!"Conflict with register key");
	}
*/
	hotkey = newHotKey(key, handler);
	insList(listHotKey, 0, hotkey);
}

void unregisterHotKey(SDLKey key)
{
	hotKey_t *hotkey;
	int my_index;

	hotkey = findHotkey(key);

	if (hotkey == NULL) {
		assert(!"This hotkey not register");
	}

	my_index = searchListItem(listHotKey, hotkey);
	assert(my_index != -1);
	delListItem(listHotKey, my_index, destroyHotKey);
}

void enableHotKey(SDLKey key)
{
	hotKey_t *hotkey;

	hotkey = findHotkey(key);

	if (hotkey == NULL) {
		assert(!"This hotkey not register");
	}

	lastActive = getMyTime();
	hotkey->active = TRUE;
}

void disableHotKey(SDLKey key)
{
	hotKey_t *hotkey;

	hotkey = findHotkey(key);

	if (hotkey == NULL) {
		assert(!"This hotkey not register");
	}

	lastActive = getMyTime();
	hotkey->active = FALSE;
}

void eventHotKey()
{
	my_time_t currentTime;
	Uint8 *mapa;
	int i;

	currentTime = getMyTime();

	if (currentTime - lastActive < HOTKEY_ACTIVE_INTERVAL) {
		return;
	}

	mapa = SDL_GetKeyState(NULL);

	for (i = 0; i < listHotKey->count; i++) {
		hotKey_t *this;

		this = (hotKey_t *) listHotKey->list[i];

		if (mapa[this->key] == SDL_PRESSED && this->active == TRUE) {
			lastActive = getMyTime();
			//printf("hotKey = %d\n", this->key);
			this->handler();

			return;
		}
	}
}

void quitHotKey()
{
	destroyListItem(listHotKey, destroyHotKey);
}
