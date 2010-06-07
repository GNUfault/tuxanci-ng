#include <SDL.h>
#include <limits.h>
#include <assert.h>

#include "main.h"
#include "control.h"

control_t *control_new(SDLKey arg_up, SDLKey arg_right, SDLKey arg_left,
		       SDLKey arg_down, SDLKey arg_shot, SDLKey arg_switch)
{
	control_t *tmp;

	tmp = malloc(sizeof(control_t));
	memset(tmp, 0, sizeof(control_t));
	tmp->key[CONTROL_UP] = arg_up;
	tmp->key[CONTROL_RIGHT] = arg_right;
	tmp->key[CONTROL_LEFT] = arg_left;
	tmp->key[CONTROL_DOWN] = arg_down;
	tmp->key[CONTROL_SHOT] = arg_shot;
	tmp->key[CONTROL_SWITCH] = arg_switch;

	return tmp;
}

int control_get_key_route(control_t *my_control)
{
	Uint8 *map;
	SDLKey ret_key;
	int z;
	int i;

	assert(my_control != NULL);

	ret_key = CONTROL_NONE;

	map = SDL_GetKeyState(NULL);

	for (i = 0; i < CONTROL_KEY_COUNT_ROUTE; i++) {
		if (map[my_control->key[i]] == SDL_PRESSED) {
			my_control->count[i]++;
		} else {
			my_control->count[i] = 0;
		}
	}

	z = INT_MAX;

	for (i = 0; i < CONTROL_KEY_COUNT_ROUTE; i++) {
		if (my_control->count[i] > 0 &&
		    my_control->count[i] < z &&
		    map[my_control->key[i]] == SDL_PRESSED) {
			z = my_control->count[i];
			ret_key = i;
		}
	}

	return ret_key;
}

int control_get_key_action(control_t *my_control)
{
	Uint8 *map;

	assert(my_control != NULL);

	map = SDL_GetKeyState(NULL);

	if (map[my_control->key[CONTROL_SHOT]] == SDL_PRESSED) {
		return CONTROL_SHOT;
	}

	if (map[my_control->key[CONTROL_SWITCH]] == SDL_PRESSED) {
		return CONTROL_SWITCH;
	}

	return CONTROL_NONE;
}

void control_destroy(control_t *my_control)
{
	assert(my_control != NULL);

	free(my_control);
}
