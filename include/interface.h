
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

//rozlisenie obrazovky
#define WINDOW_SIZE_X 800
#define WINDOW_SIZE_Y 600

//napis na okne
#define WINDOW_TITLE "Tuxanci next generation SVN revision 8"
#define WIN_BPP 0
#define USR_EVT_TIMER 0

//interval pri ktorom sa spusta akcia();
#define INTERVAL 50

extern bool_t isInterfaceInicialized();
extern int initSDL();
extern SDL_Surface* getSDL_Screen();
extern void getMousePosition(int *x, int *y);
extern int isMouseClicked();
extern void interfaceRefresh();
extern void eventSDL();
extern void quitSDL();


#endif
