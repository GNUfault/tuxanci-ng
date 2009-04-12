/*
 * keyboardBuffer.h
 *
 *  Created on: 1.8.2008
 *      Author: Karel Podvolecky
 *
 *  Jednoducha fronta stisknutych klaves.
 */

#ifndef KEYBOARD_BUFFER_H

#    define KEYBOARD_BUFFER_H

#    include <SDL.h>
#    include "main.h"

typedef struct {
	SDL_keysym *buff;			/* buffer klaves (kod dle SDLKey enumu) */
	int size;					/* aktualni velikost alokovaneho bufferu (v poctu uchovatelnych klaves) */
	int count;					/* pocet klaves aktualne v bufferu */
	int begin;					/* ukazatel na zacatek bufferu (tj. kam se pridavaji klavesy) */
	int end;					/* ukazatel na konec bufferu (tj. odkud se odebiraji klavesy) */
} keyboardBuffer_t;

extern void keyboardBuffer_init(int size);
extern void keyboardBuffer_clear();
extern bool_t keyboardBuffer_push(SDL_keysym key);
extern SDL_keysym keyboardBuffer_pop();
extern int keyboardBuffer_get_size();
extern int keyboardBuffer_get_count();
extern bool_t keyboardBuffer_is_any_key();
extern void keyboardBuffer_quit();

#endif							/* KEYBOARDBUFFER_H_ */
