#ifndef INTERFACE_H
#define INTERFACE_H

#include <stdio.h>
#include <string.h>
#include <time.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_thread.h>

#include "main.h"

#ifdef SUPPORT_OPENGL
#include <SDL2/SDL_opengl.h>
#endif

/*
#include <SDL_net.h>
*/

#define SDL_SUBSYSTEMS SDL_INIT_VIDEO|SDL_INIT_TIMER

/* the window title */
#define WINDOW_TITLE "Tuxánci " TUXANCI_VERSION
#define WIN_BPP 0
#define USR_EVT_TIMER 0

/* timing of the timer [in ms] */
#define INTERVAL		50

/* size of the keyboard buffer (in characters) */
#define KEYBOARD_BUFFER_SIZE	256

extern bool_t interface_is_inicialized();
extern bool_t interface_is_use_open_gl();
extern void interface_enable_keyboard_buffer();
extern void interface_disable_keyboard_buffer();
extern int interface_init();
extern SDL_Surface *interface_get_screen();
extern void interface_get_mouse_position(int *x, int *y);
extern void interface_window_to_logical(int winx, int winy, int *logicalx, int *logicaly);
extern int interface_is_mouse_clicket();
extern int interface_is_press_any_key();
extern void interface_refresh();
extern void interface_event();
extern void interface_quit();

#endif /* INTERFACE_H */
