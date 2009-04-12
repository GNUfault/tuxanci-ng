
#ifndef INTERFACE_H

#    define INTERFACE_H

#    include <stdio.h>
#    include <string.h>
#    include <time.h>

#    include <SDL.h>
#    include <SDL_image.h>
#    include <SDL_thread.h>
#    include "main.h"

#ifdef SUPPORT_OPENGL
#include <SDL_opengl.h>
#endif

/*
#include <SDL_net.h>
*/

#    define SDL_SUBSYSTEMS SDL_INIT_VIDEO|SDL_INIT_TIMER

// windows title
#    define WINDOW_TITLE "Tuxánci " TUXANCI_VERSION
#    define WIN_BPP 0
#    define USR_EVT_TIMER 0

// interval (in ms) of triggering akcia();
#    define INTERVAL 50

// velikost klavesoveho bufferu (ve znacich)
#    define KEYBOARD_BUFFER_SIZE 256

extern bool_t interface_is_inicialized();
extern void interface_enable_keyboard_buffer();
extern void interface_disable_keyboard_buffer();
extern int interface_init();
extern SDL_Surface *interface_get_screen();
extern void interface_get_mouse_position(int *x, int *y);
extern int interface_is_mouse_clicket();
extern int interface_is_press_any_key();
extern void interface_refresh();
extern void interface_event();
extern void interface_quit();

#endif
