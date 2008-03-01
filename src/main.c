
#include <time.h>

#include "interface.h"
#include "main.h"
#include "font.h"
#include "image.h"
#include "layer.h"
#include "tux.h"
#include "screen.h"
#include "screen_world.h"
#include "screen_mainMenu.h"
#include "screen_analyze.h"
#include "screen_setting.h"
#include "screen_gameType.h"
#include "screen_choiceArena.h"
#include "wall.h"
#include "shot.h"
#include "panel.h"
#include "myTimer.h"
#include "arenaFile.h"
#include "audio.h"
#include "sound.h"
#include "music.h"
#include "item.h"

static int my_argc;
static char **my_argv;

static void init()
{
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

	printf("quit..\n");

	exit(0);
}

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

	printf("dist dir = %s\n", DISTDIR);

	my_argc = argc;
	my_argv = argv;

	init();

	setScreen("mainMenu");

	eventSDL();
	quit();

	return 0;
}
 
