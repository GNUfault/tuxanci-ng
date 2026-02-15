#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <SDL2/SDL.h>

#include "keyboardBuffer.h"

static keyboardBuffer_t *keyboardBuffer = NULL;

static SDL_Keysym emptyKey;

/*
 * Key buffer initialization
 * the parameter sets the number of keys
 */
void keyboard_buffer_init(int size)
{
	assert(size > 0);
	assert(keyboardBuffer == NULL);

	keyboardBuffer = malloc(sizeof(keyboardBuffer_t));
	assert(keyboardBuffer != NULL);
	memset(keyboardBuffer, 0, sizeof(keyboardBuffer_t));

	keyboardBuffer->buff = malloc(size * sizeof(SDL_Keysym));
	assert(keyboardBuffer->buff != NULL);
	memset(keyboardBuffer->buff, 0, size * sizeof(SDL_Keysym));

	keyboardBuffer->begin = 0;
	keyboardBuffer->end = 0;
	keyboardBuffer->count = 0;
	keyboardBuffer->size = size;

	emptyKey.sym = SDLK_UNKNOWN;
}

/*
 * Empty the buffer
 */
void keyboard_buffer_clear()
{
	assert(keyboardBuffer != NULL);

	keyboardBuffer->begin = 0;
	keyboardBuffer->end = 0;
	keyboardBuffer->count = 0;
	memset(keyboardBuffer->buff, 0, keyboardBuffer->size * sizeof(SDL_Keysym));
}

/*
 * Adds key to the end of the buffer. If the buffer is overrun,
 * an error is printed to stderr and the key is dropped.
 */
bool_t keyboard_buffer_push(SDL_Keysym key)
{
	assert(keyboardBuffer != NULL);

	if (keyboardBuffer->count >= keyboardBuffer->size) {
		error("Keyboard buffer overrun - dropping the key [%02x]", key.sym);
		return FALSE;
	}

	keyboardBuffer->buff[keyboardBuffer->end] = key;

	keyboardBuffer->end++;
	if (keyboardBuffer->end >= keyboardBuffer->size) {
		keyboardBuffer->end = 0;
	}

	keyboardBuffer->count++;

	return TRUE;
}

/*
 * Takes out first key from the buffer and returns it. If the buffer is empty,
 * an error is printed to stderr and SDLK_UNKNOWN is returned.
 */
SDL_Keysym keyboard_buffer_pop()
{
	assert(keyboardBuffer != NULL);

	if (keyboardBuffer->count <= 0) {
		error("Keyboard buffer underrun");
		return emptyKey;
	}

	SDL_Keysym key = keyboardBuffer->buff[keyboardBuffer->begin];
	keyboardBuffer->buff[keyboardBuffer->begin] = emptyKey;

	keyboardBuffer->begin++;

	if (keyboardBuffer->begin >= keyboardBuffer->size) {
		keyboardBuffer->begin = 0;
	}

	keyboardBuffer->count--;

	return key;
}

/*
 * The buffer size
 */
int keyboard_buffer_get_size()
{
	assert(keyboardBuffer != NULL);
	return keyboardBuffer->size;
}

/*
 * Number of keys in the buffer
 */
int keyboard_buffer_get_count()
{
	assert(keyboardBuffer != NULL);
	return keyboardBuffer->count;
}


bool_t keyboard_buffer_is_any_key()
{
	return keyboard_buffer_get_count() > 0 ? TRUE : FALSE;
}

/*
 * Frees the buffer. If it is not empty, information
 * about filling is prined to stderr first.
 */
void keyboard_buffer_quit()
{
	assert(keyboardBuffer != NULL);

	if (keyboardBuffer->count > 0) {
		warning("Shutting down non-empty keyboard buffer");
	}

	free(keyboardBuffer->buff);
	free(keyboardBuffer);
	keyboardBuffer = NULL;
}
