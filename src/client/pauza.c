
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "main.h"
#include "list.h"
#include "myTimer.h"

#include "interface.h"
#include "image.h"
#include "pauza.h"
#include "hotKey.h"

static image_t *g_pauza;
static bool_t activePauza;

static void switchPauzed()
{
	if( activePauza == TRUE )
	{
		activePauza = FALSE;
	}
	else
	{
		activePauza = TRUE;
	}
}

static void hotkey_pauze()
{
	switchPauzed();
}

void initPauza()
{
	g_pauza = addImageData("pauza.png", IMAGE_ALPHA, "pauza", IMAGE_GROUP_USER);
	activePauza = FALSE;

	registerHotKey(SDLK_p, hotkey_pauze);
}

void drawPauza()
{
	if( activePauza )
	{
		drawImage(g_pauza,
			WINDOW_SIZE_X/2 - g_pauza->w/2,
			WINDOW_SIZE_Y/2 - g_pauza->h/2,
			0, 0,
			g_pauza->w, g_pauza->h);
	}
}

void eventPauza()
{
}

bool_t isPauzeActive()
{
	return activePauza;
}

void quitPauza()
{
	unregisterHotKey(SDLK_p);
}

