
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include "main.h"
#include "tux.h"

#ifndef BUBLIC_SERVER

#include "font.h"
#include "image.h"
#include "layer.h"
#include "interface.h"
#include "screen.h"
#include "screen_world.h"
#include "screen_mainMenu.h"
#include "screen_analyze.h"
#include "screen_setting.h"
#include "screen_gameType.h"
#include "screen_choiceArena.h"
#include "screen_table.h"
#include "screen_credits.h"
#include "homeDirector.h"
#include "item.h"
#include "shot.h"
#include "arenaFile.h"
#include "panel.h"
#include "audio.h"
#include "sound.h"
#include "music.h"

#endif

static int my_argc;
static char **my_argv;

#ifndef BUBLIC_SERVER

static void init()
{
	createHomeDirector();

	initSDL();
	initLayer();
	initFont("DejaVuSans.ttf", 16);
	initImageData();
	initAudio();
	initSound();
	initMusic();
	initScreen();
	initArenaFile();
	initTux();
	initItem();
	initShot();
	initPanel();
	initWorld();
	initScreenMainMenu();
	initScreenAnalyze();
	initScreenSetting();
	initScreenGameType();
	initScreenChoiceArena();
	initScreenCredits();
	initScreenTable();
}

void quit()
{
	quitSDL();
	quitLayer();
	quitFont();
	quitScreen();
	quitArenaFile();
	quitItem();
	quitTux();
	quitShot();
	quitPanel();
	quitWorld();
	quitImageData();
	quitSound();
	quitMusic();
	quitAudio();
	quitScreenMainMenu();
	quitScreenAnalyze();
	quitScreenSetting();
	quitScreenGameType();
	quitScreenChoiceArena();
	quitScreenCredits();
	quitScreenTable();

	printf("quit..\n");

	exit(0);
}

#endif

char* getParam(char *s)
{
	int i;

	for( i = 0 ; i < my_argc - 1 ; i++ )
	{
		if( strcmp(s, my_argv[i]) == 0 )
		{
			return my_argv[i+1];
		}
	}

	return NULL;
}

char* getParamElse(char *s1, char *s2)
{
	char *ret;
	ret = getParam(s1);

	if( ret == NULL)
	{
		return s2;
	}

	return ret;
}

bool_t isParamFlag(char *s)
{
	int i;

	for( i = 0 ; i < my_argc ; i++ )
	{
		if( strcmp(s, my_argv[i]) == 0 )
		{
			return TRUE;
		}
	}

	return FALSE;
}

char *getString(int n)
{
	char str[STR_NUM_SIZE];
	sprintf(str, "%d", n);
	return strdup(str);
}

int main(int argc, char *argv[])
{
	srand( (unsigned) time(NULL) );

	my_argc = argc;
	my_argv = argv;

#ifndef BUBLIC_SERVER

	init();

	setScreen("mainMenu");

	eventSDL();
	quit();
#endif

#ifdef BUBLIC_SERVER

	signal(SIGPIPE, SIG_IGN);
	signal(SIGINT, my_handler_quit);
	signal(SIGTERM, my_handler_quit);
	signal(SIGQUIT, my_handler_quit);

	if( initPublicServer() < 0 )
	{
		quitPublicServer();
		return -1;
	}

	while(1)
	{
		eventPublicServer();
	}

#endif

	return 0;
}
 
