/*
 * keyboardBuffer.c
 *
 *  Created on: 1. 8. 2008
 *      Author: Karel Podvolecky
 *
 *  Simple front of pressed keys.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <SDL.h>

#include "keyboardBuffer.h"

static keyboardBuffer_t *keyboardBuffer = NULL;

static SDL_keysym emptyKey;

/*
 * Key buffer initializes. The parameter sets the number of keys.
 */
void initKeyboardBuffer(int size)
{
	assert(size > 0);
	assert(keyboardBuffer == NULL);

	keyboardBuffer = malloc(sizeof(keyboardBuffer_t));
	assert(keyboardBuffer != NULL);
	memset(keyboardBuffer, 0, sizeof(keyboardBuffer_t));

	keyboardBuffer->buff = malloc(size * sizeof(SDL_keysym));
	assert(keyboardBuffer->buff != NULL);
	memset(keyboardBuffer->buff, 0, size * sizeof(SDL_keysym));

	keyboardBuffer->begin = 0;
	keyboardBuffer->end = 0;
	keyboardBuffer->count = 0;
	keyboardBuffer->size = size;

	emptyKey.sym = SDLK_UNKNOWN;
}

/*
 * Empty the buffer
 */
void clearKeyboardBuffer()
{
	assert(keyboardBuffer != NULL);

	keyboardBuffer->begin = 0;
	keyboardBuffer->end = 0;
	keyboardBuffer->count = 0;
	memset(keyboardBuffer->buff, 0, keyboardBuffer->size * sizeof(SDL_keysym));
}

/*
 * Adds key to the end of the buffer. If the buffer is overrun,
 * an error is printed to stderr and the key is dropped.
 */
bool_t pushKeyToKeyboardBuffer(SDL_keysym key)
{
	assert(keyboardBuffer != NULL);

	if (keyboardBuffer->count >= keyboardBuffer->size){
		fprintf(stderr, _("Keyboard Buffer overrun! Dropping key: %02x\n"), key.sym);
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
SDL_keysym popKeyFromKeyboardBuffer()
{
	assert(keyboardBuffer != NULL);

	if (keyboardBuffer->count <= 0){
		fprintf(stderr, _("Keyboard Buffer underrun!\n"));
		return emptyKey;
	}

	SDL_keysym key = keyboardBuffer->buff[keyboardBuffer->begin];
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
int getKeyboardBufferSize()
{
	assert(keyboardBuffer != NULL);
	return keyboardBuffer->size;
}

/*
 * Number of keys in the buffer
 */
int KeyboardBufferCount()
{
	assert(keyboardBuffer != NULL);
	return keyboardBuffer->count;
}


bool_t isAnyKeyInKeyboardBuffer()
{
	return KeyboardBufferCount() > 0 ? TRUE : FALSE;
}

/*
 * Frees the buffer. If it is not empty, information
 * about filling is prined to stderr first.
 */
void quitKeyboardBuffer()
{
	assert(keyboardBuffer != NULL);

	if (keyboardBuffer->count > 0){
		fprintf(stderr, _("Keyboard buffer is not empty!\n"));
	}

	free(keyboardBuffer->buff);
	free(keyboardBuffer);
	keyboardBuffer = NULL;
}

