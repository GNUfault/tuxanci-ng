
#include <stdlib.h>

#include "main.h"
#include "myTimer.h"

#include "interface.h"
#include "screen.h"
#include "keyboardBuffer.h"
#include "hotKey.h"

// window surface
static SDL_Surface *screen;
//static SDL_Surface *my_surface;

// timer
static SDL_TimerID timer;

// window flags
#ifndef SUPPORT_OPENGL
static Uint32 g_win_flags = SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_ANYFORMAT;
#endif

#ifdef SUPPORT_OPENGL
static Uint32 g_win_flags = SDL_OPENGL;
#endif

static bool_t isInterfaceInit = FALSE;

// flag, kterym se ridi zarazovani klaves do bufferu
bool_t keyboardBufferEnabled = FALSE;
bool_t isSlowHack;

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

void enableKeyboardBuffer()
{
	keyboardBufferEnabled = TRUE;
}

void disableKeyboardBuffer()
{
	keyboardBufferEnabled = FALSE;
}

void hotkey_screen()
{
	if (g_win_flags & SDL_FULLSCREEN)
		g_win_flags &= ~SDL_FULLSCREEN;
	else
		g_win_flags |= SDL_FULLSCREEN;

	if (SDL_WM_ToggleFullScreen(screen) == 0) {
		fprintf(stderr, _("Unable to switch to FULLSCREEN mode!\n"));

		SDL_FreeSurface(screen);
		screen =
			SDL_SetVideoMode(WINDOW_SIZE_X, WINDOW_SIZE_Y, WIN_BPP,
							 g_win_flags);

		if (screen == NULL) {
			fprintf(stderr, _("Unable to switch to WINDOW mode! Error: %s\n"),
					SDL_GetError());
			return;
		}
	}
}

#ifdef SUPPORT_OPENGL
/*
 * Sets video mode and sets up OpenGL for this change
 */
SDL_Surface * SetVideoMode(int width, int height, int bpp, Uint32 flags)
{
	SDL_Surface *rval = 0;

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	rval = SDL_SetVideoMode(width, height, bpp, flags);

	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glClearDepth(1.0);
	glDisable(GL_DEPTH_TEST);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, 1);

	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glOrtho(0.0f,width, height,0.0f,-1.0f,1.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glEnable(GL_TEXTURE_2D);

	return rval;
}
#endif

int initSDL()
{
#ifdef DEBUG
	DEBUG_MSG(_("Initializing SDL system\n"));
#endif
	// initialization of SDL
	if (SDL_Init(SDL_SUBSYSTEMS) == -1) {
		fprintf(stderr, "%s\n", SDL_GetError());
		SDL_Quit();
		return -1;
	}

#ifndef SUPPORT_OPENGL
	screen = SDL_SetVideoMode(WINDOW_SIZE_X, WINDOW_SIZE_Y, 0, g_win_flags);
#endif

#ifdef SUPPORT_OPENGL
	screen = SetVideoMode(WINDOW_SIZE_X, WINDOW_SIZE_Y, 0, g_win_flags);
#endif

	if (screen == NULL) {
		fprintf(stderr, "%s\n", SDL_GetError());
		SDL_Quit();
		return -1;
	}

	SDL_WM_SetCaption(WINDOW_TITLE, NULL);
	//SDL_ShowCursor(0);
	SDL_EnableKeyRepeat(0, 1);
	timer = SDL_AddTimer(INTERVAL, TimerCallback, NULL);

	initKeyboardBuffer(KEYBOARD_BUFFER_SIZE);

	initHotKey();
	registerHotKey(SDLK_F1, hotkey_screen);

	srand((unsigned) time(NULL));
	isInterfaceInit = TRUE;

	return 0;
}

SDL_Surface *getSDL_Screen()
{
	//return my_surface;
	return screen;
}

void interfaceRefresh()
{
#ifndef SUPPORT_OPENGL
	SDL_Flip(screen);
#endif

#ifdef SUPPORT_OPENGL
	SDL_GL_SwapBuffers();
#endif
}

void getMousePosition(int *x, int *y)
{
	SDL_GetMouseState(x, y);
}

int isMouseClicked()
{
	return SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT);
}

int isPressAnyKey()
{
	Uint8 *mapa;
	int i;

	mapa = SDL_GetKeyState(NULL);

	for (i = SDLK_BACKSPACE; i < SDLK_KP9; i++)
		if (mapa[i] == SDL_PRESSED) {
			return 1;
		}

	return 0;
}

void printPressAnyKey()
{
	Uint8 *mapa;
	int i;

	mapa = SDL_GetKeyState(NULL);

	for (i = SDLK_BACKSPACE; i < SDLK_KP9; i++)
		if (mapa[i] == SDL_PRESSED) {
			printf(_("Pressed key with SDL value: %d\n"), i);
		}

}


int hack_slow()
{
	static time_t lastTime = 0;
	static bool_t isSlowHack = FALSE;
	time_t currentTime;

	currentTime = getMyTime();

	if( lastTime == 0 )
	{
		lastTime = currentTime;
		return 0;
	}

	//printf("DEBUG: time interval %d\n", currentTime - lastTime);

	if( isSlowHack == FALSE && currentTime - lastTime >= 100 )
	{
		//printf("start slow hack (%d)\n", currentTime - lastTime);
		isSlowHack = TRUE;
		lastTime = getMyTime();
		return 1;
	}

	if( isSlowHack == TRUE && currentTime - lastTime >= 50 )
	{
		//printf("stop slow hack (%d)\n", currentTime - lastTime);
		isSlowHack = FALSE;
		lastTime = getMyTime();
		return 0;
	}

	if( isSlowHack == TRUE )
	{
		//printf("hack time interval %d\n", currentTime - lastTime);
	}

	lastTime = getMyTime();

	return isSlowHack;
}

int eventAction()
{
	SDL_Event event;


	while (SDL_WaitEvent(&event)) {
		switch (event.type) {
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym) {
			default:
				//neni potreba vyuzivat frontu stale
				if (keyboardBufferEnabled == TRUE)
					pushKeyToKeyboardBuffer(event.key.keysym);
				break;
			}
			break;

		case SDL_USEREVENT:
			switch (event.user.code) {
			case USR_EVT_TIMER:
				if( hack_slow() )
				{
					break;
				}

				drawScreen();
				eventScreen();
				eventHotKey();
				switchScreen();
				break;

			default:
				break;
			}
			break;

			// change of window size
		case SDL_VIDEORESIZE:
			screen =
				SDL_SetVideoMode(event.resize.w, event.resize.h, WIN_BPP,
								 g_win_flags);

			if (screen == NULL) {
				fprintf(stderr,
						_("Unable to change WINDOW resolution! Error: %s\n"),
						SDL_GetError());
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
	while (1) {
		if (eventAction() == -1)
			return;
	}
}

void quitSDL()
{
	DEBUG_MSG(_("Quitting SDL\n"));

	quitHotKey();
	quitKeyboardBuffer();

	SDL_Quit();

	isInterfaceInit = FALSE;
}
