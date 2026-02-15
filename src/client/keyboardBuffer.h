#ifndef KEYBOARD_BUFFER_H
#define KEYBOARD_BUFFER_H

#include <SDL2/SDL.h>
#include "main.h"

typedef struct {
	SDL_Keysym *buff;			/* buffer of keys (code according to the SDLKey enum) */
	int size;				/* up-to-date size of allocated buffer (size == number of saveable keys) */
	int count;				/* up-to-date number of keys saved in the buffer */
	int begin;				/* pointer to the beginning of the buffer (i.e. where to add new keys to) */
	int end;				/* pointer to the end of the buffer (i.e. where to remove old keys from) */
} keyboardBuffer_t;

extern void keyboard_buffer_init(int size);
extern void keyboard_buffer_clear();
extern bool_t keyboard_buffer_push(SDL_Keysym key);
extern SDL_Keysym keyboard_buffer_pop();
extern int keyboard_buffer_get_size();
extern int keyboard_buffer_get_count();
extern bool_t keyboard_buffer_is_any_key();
extern void keyboard_buffer_quit();

#endif /* KEYBOARD_BUFFER_H */
