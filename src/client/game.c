
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
#include "homeDirector.h"
#include "item.h"
#include "shot.h"
#include "arenaFile.h"

#include "image.h"
#include "layer.h"
#include "interface.h"
#include "screen.h"
#include "font.h"
#include "settingKeys.h"
#include "panel.h"
#include "radar.h"

#ifndef NO_SOUND
#    include "audio.h"
#    include "sound.h"
#    include "music.h"
#endif

#include "world.h"
#include "mainMenu.h"
#include "analyze.h"
#include "setting.h"
#include "settingKeys.h"
#include "gameType.h"
#include "downArena.h"
#include "choiceArena.h"
#include "table.h"
#include "credits.h"
#include "browser.h"

static void initGame()
{
	createHomeDirector();

	initSDL();
	initFont();
	initLayer();
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
	initScreenChoiceArena();
	initScreenSetting();
	initScreenSettingKeys();
	initScreenGameType();
	initScreenDownArena();
	initScreenCredits();
	initScreenTable();
	initScreenBrowser();
}

void quitGame()
{
	quitSDL();

	quitScreenMainMenu();
	quitScreenAnalyze();
	quitScreenSetting();
	quitScreenSettingKeys();
	quitScreenGameType();
	quitScreenDownArena();
	quitScreenChoiceArena();
	quitScreenCredits();
	quitScreenTable();
	quitScreenBrowser();

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
#ifndef NO_SOUND
	quitSound();
	quitMusic();
	quitAudio();
#endif

	DEBUG_MSG(_("Quitting TUXANCI...\n"));

	exit(0);
}

void startGame()
{
	initGame();

	startScreen("mainMenu");

	eventSDL();
	quitGame();
}
