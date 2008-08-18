
#ifndef INTERFACE_H

#define INTERFACE_H

#include <stdio.h>
#include <string.h>
#include <time.h>

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_thread.h>
#include "main.h"

/*
#include <SDL_net.h>
*/

#define SDL_SUBSYSTEMS SDL_INIT_VIDEO|SDL_INIT_TIMER|SDL_DOUBLEBUF

// windows title
#define WINDOW_TITLE "Tuxánci " TUXANCI_VERSION
#define WIN_BPP 0
#define USR_EVT_TIMER 0

// interval (in ms) of triggering akcia();
#define INTERVAL 50

// velikost klavesoveho bufferu (ve znacich)
#define KEYBOARD_BUFFER_SIZE 256

extern bool_t isInterfaceInicialized();
extern void enableKeyboardBuffer();
extern void disableKeyboardBuffer();
extern int initSDL();
extern SDL_Surface* getSDL_Screen();
extern void getMousePosition(int *x, int *y);
extern int isMouseClicked();
extern int isPressAnyKey();
extern void interfaceRefresh();
extern void eventSDL();
extern void quitSDL();

#endif
