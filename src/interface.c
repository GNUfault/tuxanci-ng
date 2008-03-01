
#include "main.h"
#include "interface.h"
#include "screen.h"

//povrch okna
static SDL_Surface *screen;
static SDL_Surface *fake_screen;

//casovac
static SDL_TimerID timer;

//priznaky okna
static Uint32 g_win_flags = SDL_HWSURFACE|SDL_DOUBLEBUF;

static bool_t isInterfaceInit = FALSE;

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

int initSDL()
{
	printf("init SDL..\n");

	//inicializujem SDL
	if( SDL_Init(SDL_SUBSYSTEMS) == -1 )
	{
		fprintf(stderr, "%s\n", SDL_GetError());
		SDL_Quit();
		return -1;
	}

	//inicializujem okno
	//screen = SDL_SetVideoMode(WINDOW_SIZE_X, WINDOW_SIZE_Y, 0, g_win_flags);
	screen = SDL_SetVideoMode(WINDOW_SIZE_X, WINDOW_SIZE_Y, 0, g_win_flags);

        fake_screen = SDL_CreateRGBSurface(screen->flags, WINDOW_SIZE_X, WINDOW_SIZE_Y,
		screen->format->BitsPerPixel,
		screen->format->Rmask,	screen->format->Gmask,
		screen->format->Bmask,	screen->format->Amask);

	if (screen == NULL )
	{
		fprintf(stderr, "%s\n", SDL_GetError());
		SDL_Quit();
		return -1;
	}

	SDL_WM_SetCaption(WINDOW_TITLE, NULL);
	//SDL_ShowCursor(0);
	//SDL_EnableKeyRepeat(1,1);
	timer = SDL_AddTimer(INTERVAL, TimerCallback, NULL);

	srand((unsigned)time(NULL));
	isInterfaceInit = TRUE;

	return 0;
}

SDL_Surface* getSDL_Screen()
{
	//return fake_screen;
	return screen;
}

void interfaceRefresh()
{
	//SDL_BlitSurface(fake_screen, NULL, screen, NULL);
	//SDL_UpdateRect(screen, 0, 0, screen->w, screen->h);
	SDL_Flip(screen);
}

int toggleFullscreen()
{
	if(g_win_flags & SDL_FULLSCREEN)
		g_win_flags &= ~SDL_FULLSCREEN;
	else
		g_win_flags |= SDL_FULLSCREEN;

	if( SDL_WM_ToggleFullScreen(screen) == 0 )
	{
		fprintf(stderr, "Nemozem prepnut do fullscreenu!\n");

		SDL_FreeSurface(screen);
		screen = SDL_SetVideoMode(WINDOW_SIZE_X, WINDOW_SIZE_Y, WIN_BPP, g_win_flags);

		if(screen == NULL)
		{
			fprintf(stderr, "Nemozem premnut do okna:%s\n", SDL_GetError());
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
				break;
			}
		break;

		case SDL_USEREVENT:
			switch(event.user.code)
			{
				case USR_EVT_TIMER:
					drawScreen();
					eventScreen();
				break;

				default:
				break;
			}
		break;

		// Zmena velikosti okna
		case SDL_VIDEORESIZE:
			screen = SDL_SetVideoMode(event.resize.w,event.resize.h, WIN_BPP, g_win_flags);

			if(screen == NULL)
			{
				fprintf(stderr, "Nemozem zmenit rozlisene okna: %s\n",SDL_GetError());
				return 0;
			}
		break;

		// Pozadavek na ukonceni
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
	printf("quit SDL..\n");
	SDL_Quit();
	isInterfaceInit = FALSE;
}
