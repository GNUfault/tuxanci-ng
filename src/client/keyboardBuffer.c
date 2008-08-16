/*
 * keyboardBuffer.c
 *
 *  Created on: 1.8.2008
 *      Author: Karel Podvolecky
 *
 *  Jednoducha fronta stisknutych klaves.
 *
 */

#include <stdio.h>
#include <assert.h>
#include <SDL.h>

#include "keyboardBuffer.h"

static keyboardBuffer_t *keyboardBuffer=NULL;

static SDL_keysym emptyKey;

/*
 * Inicializuje klavesovy buffer. Parametr udava pocet klaves.
 */
void initKeyboardBuffer(int size){
	assert(size>0);
	assert(keyboardBuffer==NULL);

	keyboardBuffer = malloc(sizeof(keyboardBuffer_t));
	assert(keyboardBuffer!=NULL);
	memset(keyboardBuffer, 0, sizeof(keyboardBuffer_t));

	keyboardBuffer->buff = malloc(size * sizeof(SDL_keysym));
	assert(keyboardBuffer->buff!=NULL);
	memset(keyboardBuffer->buff, 0, size * sizeof(SDL_keysym));

	keyboardBuffer->begin=0;
	keyboardBuffer->end=0;
	keyboardBuffer->count=0;
	keyboardBuffer->size=size;

	emptyKey.sym = SDLK_UNKNOWN;
}

/*
 * Vyprazdni buffer.
 */
void clearKeyboardBuffer(){
	assert(keyboardBuffer!=NULL);

	keyboardBuffer->begin=0;
	keyboardBuffer->end=0;
	keyboardBuffer->count=0;
	memset(keyboardBuffer->buff, 0, keyboardBuffer->size * sizeof(SDL_keysym));
}

/*
 * Prida klavesu na konec bufferu. Pokud je buffer preplnen,
 * vypise chybu na chybovy vystup a klavesu zahodi.
 */
bool_t pushKeyToKeyboardBuffer(SDL_keysym key){
	assert(keyboardBuffer!=NULL);

	if (keyboardBuffer->count >= keyboardBuffer->size){
		fprintf(stderr, "Preteceni klavesoveho bufferu! Zahazuji klavesu: %02x\n", key.sym);
		return FALSE;
	}

	keyboardBuffer->buff[keyboardBuffer->end] = key;

	keyboardBuffer->end++;
	if (keyboardBuffer->end>=keyboardBuffer->size)
		keyboardBuffer->end=0;

	keyboardBuffer->count++;

	return TRUE;
}

/*
 * Vyjme prvni klavesu z bufferu a vrati ji. Pokud je buffer prazdny,
 * vypise chybu na chybovy  vystup a vrati SDLK_UNKNOWN
 */
SDL_keysym popKeyFromKeyboardBuffer(){
	assert(keyboardBuffer!=NULL);

	if (keyboardBuffer->count <= 0){
		fprintf(stderr, "Podteceni klavesoveho bufferu!\n");
		return emptyKey;
	}

	SDL_keysym key = keyboardBuffer->buff[keyboardBuffer->begin];
	keyboardBuffer->buff[keyboardBuffer->begin]=emptyKey;

	keyboardBuffer->begin++;
	if (keyboardBuffer->begin>=keyboardBuffer->size)
		keyboardBuffer->begin=0;

	keyboardBuffer->count--;

	return key;
}

/*
 * Velikost bufferu.
 */
int getKeyboardBufferSize(){
	assert(keyboardBuffer!=NULL);
	return keyboardBuffer->size;
}

/*
 * Pocet klaves v bufferu.
 */
int KeyboardBufferCount(){
	assert(keyboardBuffer!=NULL);
	return keyboardBuffer->count;
}


bool_t isAnyKeyInKeyboardBuffer(){
	return KeyboardBufferCount()>0 ? TRUE : FALSE;
}

/*
 * Uvolni buffer. Pokud neni prazdny, vypise predtim
 * info o zaplneni na chybovy vystup.
 */
void quitKeyboardBuffer(){
	assert(keyboardBuffer!=NULL);

	if (keyboardBuffer->count>0){
		fprintf(stderr, "Klavesovy buffer neni prazdny!\n");
	}

	free(keyboardBuffer->buff);
	free(keyboardBuffer);
	keyboardBuffer=NULL;
}
