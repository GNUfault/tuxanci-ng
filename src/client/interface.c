#include <stdlib.h>

#include "main.h"
#include "myTimer.h"

#include "interface.h"
#include "screen.h"
#include "keyboardBuffer.h"
#include "hotKey.h"


static SDL_Surface *screen;	/* window surface */
/*static SDL_Surface *my_surface;*/
static SDL_TimerID timer;	/* timer */
static Uint32 g_win_flags;	/* window flags */

static bool_t isInterfaceInit = FALSE;
static bool_t keyboardBufferEnabled = FALSE;	/* flag that controls organising keys into the buffer */
static bool_t use_open_gl;

bool_t interface_is_use_open_gl()
{
	return use_open_gl;
}

bool_t interface_is_inicialized()
{
	return isInterfaceInit;
}

static Uint32 TimerCallback(Uint32 interval, void *param)
{
	SDL_Event event;

	UNUSED(param);

	event.type = SDL_USEREVENT;
	event.user.code = USR_EVT_TIMER;
	event.user.data1 = NULL;
	event.user.data2 = NULL;

	SDL_PushEvent(&event);

	return interval;
}

void interface_enable_keyboard_buffer()
{
	keyboardBufferEnabled = TRUE;
}

void interface_disable_keyboard_buffer()
{
	keyboardBufferEnabled = FALSE;
}

void hotkey_screen()
{
	if (g_win_flags & SDL_FULLSCREEN) {
		g_win_flags &= ~SDL_FULLSCREEN;
	} else {
		g_win_flags |= SDL_FULLSCREEN;
	}

	if (SDL_WM_ToggleFullScreen(screen) == 0) {
		fprintf(stderr, _("[Error] Unable to switch to the fullscreen mode\n"));

		SDL_FreeSurface(screen);
		screen = SDL_SetVideoMode(WINDOW_SIZE_X, WINDOW_SIZE_Y, WIN_BPP, g_win_flags);

		if (screen == NULL) {
			fprintf(stderr, _("[Error] Unable to switch to the window mode: %s\n"),
				SDL_GetError());
			return;
		}
	}
}

#ifdef SUPPORT_OPENGL
/*
 * Sets video mode and sets up OpenGL for this change
 */
SDL_Surface *SetVideoMode(int width, int height, int bpp, Uint32 flags)
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
#ifdef SDL_GL_SWAP_CONTROL
	SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, 1);
#endif /* SDL_GL_SWAP_CONTROL */

	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glOrtho(0.0f, width, height, 0.0f, -1.0f, 1.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glEnable(GL_TEXTURE_2D);

	return rval;
}
#endif /* SUPPORT_OPENGL */

int interface_init()
{
	DEBUG_MSG(_("[Debug] Initializing SDL\n"));

	/* initialization of SDL */
	if (SDL_Init(SDL_SUBSYSTEMS) == -1) {
		fprintf(stderr, _("[Error] Unable to initialize SDL: %s\n"), SDL_GetError());
		SDL_Quit();
		return -1;
	}

#ifndef SUPPORT_OPENGL
	use_open_gl = FALSE;
	g_win_flags = SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_ANYFORMAT;

	screen = SDL_SetVideoMode(WINDOW_SIZE_X, WINDOW_SIZE_Y, 0, g_win_flags);
#else /* SUPPORT_OPENGL */
	use_open_gl = !isParamFlag("--disable-opengl");

	if (interface_is_use_open_gl()) {
		g_win_flags = SDL_OPENGL;
		screen = SetVideoMode(WINDOW_SIZE_X, WINDOW_SIZE_Y, 0, g_win_flags);

		if (screen == NULL) {
			use_open_gl = FALSE;
			g_win_flags = SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_ANYFORMAT;
			
			screen = SDL_SetVideoMode(WINDOW_SIZE_X, WINDOW_SIZE_Y, 0, g_win_flags);
		}
	} else {
		g_win_flags = SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_ANYFORMAT;
		screen = SDL_SetVideoMode(WINDOW_SIZE_X, WINDOW_SIZE_Y, 0, g_win_flags);
	}
#endif /* SUPPORT_OPENGL */

	if (screen == NULL) {
		fprintf(stderr, _("[Error] Unable to create interface: %s\n"), SDL_GetError());
		SDL_Quit();
		return -1;
	}
	/*
	 * enable unicode by default for keyboard support - it is bit more
	 * consuming than the nonsdl but can draw more characters
	*/
	SDL_EnableUNICODE(1);
	SDL_WM_SetCaption(WINDOW_TITLE, NULL);

	/* keyboard repeating */
	SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);
	timer = SDL_AddTimer(INTERVAL, TimerCallback, NULL);

	keyboard_buffer_init(KEYBOARD_BUFFER_SIZE);

	hot_key_init();
	hot_key_register(SDLK_F1, hotkey_screen);

	srand((unsigned) time(NULL));
	isInterfaceInit = TRUE;

	return 0;
}

SDL_Surface *interface_get_screen()
{
	/*return my_surface;*/
	return screen;
}

void interface_refresh()
{
#ifndef SUPPORT_OPENGL
	SDL_Flip(screen);
#else /* SUPPORT_OPENGL */
	if (interface_is_use_open_gl()) {
		SDL_GL_SwapBuffers();
	} else {
		SDL_Flip(screen);
	}
#endif /* SUPPORT_OPENGL */
}

void interface_get_mouse_position(int *x, int *y)
{
	SDL_GetMouseState(x, y);
}

int interface_is_mouse_clicket()
{
	return SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT);
}

int interface_is_press_any_key()
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
			printf(_("[Debug] Pressed key [SDL: %d]\n"), i);
		}

}

int hack_slow()
{
	static time_t lastTime = 0;
	static bool_t isSlowHack = FALSE;
	time_t currentTime;

	currentTime = timer_get_current_time();

	if (lastTime == 0) {
		lastTime = currentTime;
		return 0;
	}

	/*printf("DEBUG: time interval %d\n", currentTime - lastTime);*/

	if (isSlowHack == FALSE && currentTime - lastTime >= 100) {
		/*printf("start slow hack (%d)\n", currentTime - lastTime);*/
		isSlowHack = TRUE;
		lastTime = timer_get_current_time();
		return 1;
	}

	if (isSlowHack == TRUE && currentTime - lastTime >= 50) {
		/*printf("stop slow hack (%d)\n", currentTime - lastTime);*/
		isSlowHack = FALSE;
		lastTime = timer_get_current_time();
		return 0;
	}

	if (isSlowHack == TRUE) {
		/*printf("hack time interval %d\n", currentTime - lastTime);*/
	}

	lastTime = timer_get_current_time();

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
						/* it is not necessary to always use the buffer */
						if (keyboardBufferEnabled == TRUE) {
							keyboard_buffer_push(event.key.keysym);
						}
						break;
				}
				break;

			case SDL_USEREVENT:
				switch (event.user.code) {
					case USR_EVT_TIMER:
						if (hack_slow()) {
							break;
						}

						screen_draw();
						screen_event();
						hot_key_event();
						screen_switch();
						break;

					default:
						break;
				}
				break;

			/* change the window size */
			case SDL_VIDEORESIZE:
				screen = SDL_SetVideoMode(event.resize.w, event.resize.h,
							  WIN_BPP, g_win_flags);

				if (screen == NULL) {
					fprintf(stderr, _("[Error] Unable to change resolution of the window: %s\n"),
						SDL_GetError());
					return 0;
				}
				break;

			/* termination request */
			case SDL_QUIT:
				return -1;
				break;

			default:
				break;
		}
	}

	return 0;
}

void interface_event()
{
	while (1) {
		if (eventAction() == -1) {
			return;
		}
	}
}

void interface_quit()
{
	DEBUG_MSG(_("[Debug] Shutting down SDL\n"));

	hot_key_quit();
	keyboard_buffer_quit();

	SDL_Quit();

	isInterfaceInit = FALSE;
}
