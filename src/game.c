
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <assert.h>
#include <sys/stat.h>

#include "main.h"
#include "tux.h"

#include "font.h"
#include "keytable.h"
#include "language.h"
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

static void initGame()
{
	createHomeDirector();

	if( initLanguage() == -1 )
	{
		printf("fatal error !\n");
		exit(-1);
	}

	initSDL();

	initLayer();
	initFont(getLanguageFont(), getLanguageSize());
	initKeyTable();
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

void quitGame()
{
	quitSDL();
	quitLayer();
	quitFont();
	quitKeyTable();
	quitLanguage();
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

void startGame()
{
	initGame();

	setScreen("mainMenu");

	eventSDL();
	quitGame();
}
