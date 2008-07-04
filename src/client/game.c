
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <assert.h>
#include <sys/stat.h>

#include "base/main.h"
#include "base/tux.h"
#include "base/homeDirector.h"
#include "base/item.h"
#include "base/shot.h"
#include "base/arenaFile.h"

#include "client/image.h"
#include "client/layer.h"
#include "client/interface.h"
#include "client/screen.h"
#include "client/font.h"
#include "client/keytable.h"
#include "client/language.h"
#include "client/panel.h"
#include "client/radar.h"

#include "audio/audio.h"
#include "audio/sound.h"
#include "audio/music.h"

#include "screen/screen_world.h"
#include "screen/screen_mainMenu.h"
#include "screen/screen_analyze.h"
#include "screen/screen_setting.h"
#include "screen/screen_gameType.h"
#include "screen/screen_choiceArena.h"
#include "screen/screen_table.h"
#include "screen/screen_credits.h"
#include "screen/screen_browser.h"

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
#ifndef NO_SOUND
	initAudio();
	initSound();
	initMusic();
#endif
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
	initScreenBrowser();
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
#ifndef NO_SOUND
	quitSound();
	quitMusic();
	quitAudio();
#endif
	quitScreenMainMenu();
	quitScreenAnalyze();
	quitScreenSetting();
	quitScreenGameType();
	quitScreenChoiceArena();
	quitScreenCredits();
	quitScreenTable();
	quitScreenBrowser();

	destroyList(listHelp);

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
