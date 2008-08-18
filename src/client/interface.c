
#include <stdlib.h>

#include "main.h"
#include "myTimer.h"

#include "interface.h"
#include "screen.h"
#include "keyboardBuffer.h"

// window surface
static SDL_Surface *screen;
//static SDL_Surface *my_surface;

// timer
static SDL_TimerID timer;

// window flags
static Uint32 g_win_flags = SDL_SWSURFACE|SDL_DOUBLEBUF|SDL_ANYFORMAT;

static bool_t isInterfaceInit = FALSE;

// flag, kterym se ridi zarazovani klaves do bufferu
bool_t keyboardBufferEnabled = FALSE;

bool_t isInterfaceInicialized()
{
	return isInterfaceInit;
}

static Uint32 TimerCallback(Uint32 interval, void *param)
{
	SDL_Event event;
	event.type = SDL_USEREVENT;
	event.user.code = USR_EVT_TIMER;
	event.user.data1 = NULL;
	event.user.data2 = NULL;

	SDL_PushEvent(&event);

	return interval;
}

void enableKeyboardBuffer(){
	keyboardBufferEnabled=TRUE;
}

void disableKeyboardBuffer(){
	keyboardBufferEnabled=FALSE;
}

int initSDL()
{
#ifdef DEBUG
	printf(_("Initializing SDL system\n"));
#endif
	// initialization of SDL
	if( SDL_Init(SDL_SUBSYSTEMS) == -1 )
	{
		fprintf(stderr, "%s\n", SDL_GetError());
		SDL_Quit();
		return -1;
	}

	// initialization of SDL
	//screen = SDL_SetVideoMode(WINDOW_SIZE_X, WINDOW_SIZE_Y, 0, g_win_flags);
	screen = SDL_SetVideoMode(WINDOW_SIZE_X, WINDOW_SIZE_Y, 0, g_win_flags);

/*
        my_surface = SDL_CreateRGBSurface(screen->flags, WINDOW_SIZE_X, WINDOW_SIZE_Y,
		screen->format->BitsPerPixel,
		screen->format->Rmask,	screen->format->Gmask,
		screen->format->Bmask,	screen->format->Amask);
*/
	//memset(my_surface->pixels, 128, 200);

	if (screen == NULL )
	{
		fprintf(stderr, "%s\n", SDL_GetError());
		SDL_Quit();
		return -1;
	}

	SDL_WM_SetCaption(WINDOW_TITLE, NULL);
	//SDL_ShowCursor(0);
	SDL_EnableKeyRepeat(0,1);
	timer = SDL_AddTimer(INTERVAL, TimerCallback, NULL);

	initKeyboardBuffer(KEYBOARD_BUFFER_SIZE);

	srand((unsigned)time(NULL));
	isInterfaceInit = TRUE;

	return 0;
}

SDL_Surface* getSDL_Screen()
{
	//return my_surface;
	return screen;
}

void interfaceRefresh()
{
	//SDL_BlitSurface(my_surface, NULL, screen, NULL);
	//SDL_UpdateRect(screen, 0, 0, screen->w, screen->h);

	//drawImage(my_surface, 200, 100, 0, 0, 200, 200);
	SDL_Flip(screen);
	//SDL_UpdateRect(screen, 400, 0, 400, 600);
}

int toggleFullscreen()
{
	if(g_win_flags & SDL_FULLSCREEN)
		g_win_flags &= ~SDL_FULLSCREEN;
	else
		g_win_flags |= SDL_FULLSCREEN;

	if( SDL_WM_ToggleFullScreen(screen) == 0 )
	{
		fprintf(stderr, _("Unable to switch to FULLSCREEN mode!\n"));

		SDL_FreeSurface(screen);
		screen = SDL_SetVideoMode(WINDOW_SIZE_X, WINDOW_SIZE_Y, WIN_BPP, g_win_flags);

		if(screen == NULL)
		{
			fprintf(stderr, _("Unable to switch to WINDOW mode! Error: %s\n"), SDL_GetError());
			return 0;
		}
	}

	return 1;
}

void getMousePosition(int *x, int *y)
{
	SDL_GetMouseState(x, y);
}

int isMouseClicked()
{
	return SDL_GetMouseState(NULL,NULL) & SDL_BUTTON(SDL_BUTTON_LEFT);
}

int isPressAnyKey()
{
	Uint8 *mapa;
	int i;

	mapa = SDL_GetKeyState(NULL);

	for( i = SDLK_BACKSPACE ; i < SDLK_KP9 ; i++ )
		if( mapa[i] == SDL_PRESSED )
		{
			return 1;
		}

	return 0;
}

void printPressAnyKey()
{
	Uint8 *mapa;
	int i;

	mapa = SDL_GetKeyState(NULL);

	for( i = SDLK_BACKSPACE ; i < SDLK_KP9 ; i++ )
		if( mapa[i] == SDL_PRESSED )
		{
			printf(_("Pressed key with SDL value: %d\n"), i);
		}

}


/*
static void action_esc()
{
	if( strcmp(getScreen(), "world") == 0 )
	{
		setScreen("analyze");
	}

	if( strcmp(getScreen(), "mainMenu") == 0 )
	{
		quit();
	}

}
*/

int eventAction()
{
	SDL_Event event;


	while(SDL_WaitEvent(&event))
	{
		switch(event.type)
		{
		case SDL_KEYDOWN:
			switch(event.key.keysym.sym)
			{
/*
				case SDLK_ESCAPE:
					action_esc();
					return 0;
				break;
*/
				case SDLK_F1:
					if(!toggleFullscreen())return 0;
				break;

				default:
					//neni potreba vyuzivat frontu stale
					if (keyboardBufferEnabled==TRUE)
						pushKeyToKeyboardBuffer(event.key.keysym);
				break;
			}
		break;

		case SDL_USEREVENT:
			switch(event.user.code)
			{
				case USR_EVT_TIMER:
					drawScreen();
					eventScreen();
					switchScreen();
				break;

				default:
				break;
			}
		break;

		// change of window size
		case SDL_VIDEORESIZE:
			screen = SDL_SetVideoMode(event.resize.w,event.resize.h, WIN_BPP, g_win_flags);

			if(screen == NULL)
			{
				fprintf(stderr, _("Unable to change WINDOW resolution! Error: %s\n"),SDL_GetError());
				return 0;
			}
		break;

		// termination request
		case SDL_QUIT:
			return -1;
		break;

		default:
		break;
		}
	}

	return 0;
}

void eventSDL()
{
	while(1)
	{
		if ( eventAction() == -1 )return;
	}
}

void quitSDL()
{
#ifdef DEBUG
	printf(_("Quitting SDL\n"));
#endif
	quitKeyboardBuffer();

	SDL_Quit();

	isInterfaceInit = FALSE;
}
