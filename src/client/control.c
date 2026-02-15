#include <SDL2/SDL.h>
#include <limits.h>
#include <assert.h>

#include "main.h"
#include "control.h"

control_t *control_new(SDL_Keycode arg_up, SDL_Keycode arg_right, SDL_Keycode arg_left,
			   SDL_Keycode arg_down, SDL_Keycode arg_shot, SDL_Keycode arg_switch)
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
	const Uint8 *state;
	int numkeys = 0;
	int ret_key;
	int z;
	int i;

	assert(my_control != NULL);

	ret_key = CONTROL_NONE;

	state = SDL_GetKeyboardState(&numkeys);

	for (i = 0; i < CONTROL_KEY_COUNT_ROUTE; i++) {
		SDL_Scancode sc = SDL_GetScancodeFromKey(my_control->key[i]);
		if (sc < numkeys && state[sc]) {
			my_control->count[i]++;
		} else {
			my_control->count[i] = 0;
		}
	}

	z = INT_MAX;

	for (i = 0; i < CONTROL_KEY_COUNT_ROUTE; i++) {
		SDL_Scancode sc = SDL_GetScancodeFromKey(my_control->key[i]);
		if (my_control->count[i] > 0 &&
			my_control->count[i] < z &&
			sc < numkeys && state[sc]) {
			z = my_control->count[i];
			ret_key = i;
		}
	}

	return ret_key;
}

int control_get_key_action(control_t *my_control)
{
	int numkeys = 0;
	const Uint8 *state = SDL_GetKeyboardState(&numkeys);

	assert(my_control != NULL);

	if (SDL_GetScancodeFromKey(my_control->key[CONTROL_SHOT]) < numkeys &&
		state[SDL_GetScancodeFromKey(my_control->key[CONTROL_SHOT])]) {
		return CONTROL_SHOT;
	}

	if (SDL_GetScancodeFromKey(my_control->key[CONTROL_SWITCH]) < numkeys &&
		state[SDL_GetScancodeFromKey(my_control->key[CONTROL_SWITCH])]) {
		return CONTROL_SWITCH;
	}

	return CONTROL_NONE;
}

void control_destroy(control_t *my_control)
{
	assert(my_control != NULL);

	free(my_control);
}
