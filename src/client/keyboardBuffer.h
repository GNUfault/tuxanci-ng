/*
 * keyboardBuffer.h
 *
 *  Created on: 1.8.2008
 *      Author: Karel Podvolecky
 *
 *  Jednoducha fronta stisknutych klaves.
 */

#ifndef KEYBOARD_BUFFER_H

#define KEYBOARD_BUFFER_H

#include <SDL.h>
#include "main.h"

typedef struct
{
    SDL_keysym *buff;           /* buffer klaves (kod dle SDLKey enumu) */
    int size;                   /* aktualni velikost alokovaneho bufferu (v poctu uchovatelnych klaves) */
    int count;                  /* pocet klaves aktualne v bufferu */
    int begin;                  /* ukazatel na zacatek bufferu (tj. kam se pridavaji klavesy) */
    int end;                    /* ukazatel na konec bufferu (tj. odkud se odebiraji klavesy) */
} keyboardBuffer_t;

extern void initKeyboardBuffer(int size);
extern void clearKeyboardBuffer();
extern bool_t pushKeyToKeyboardBuffer(SDL_keysym key);
extern SDL_keysym popKeyFromKeyboardBuffer();
extern int getKeyboardBufferSize();
extern int KeyboardBufferCount();
extern bool_t isAnyKeyInKeyboardBuffer();
extern void quitKeyboardBuffer();

#endif /* KEYBOARDBUFFER_H_ */
