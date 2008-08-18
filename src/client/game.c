
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
#include "screen_settingKeys.h"
#include "panel.h"
#include "radar.h"

#ifndef NO_SOUND
#include "audio.h"
#include "sound.h"
#include "music.h"
#endif

#include "screen_world.h"
#include "screen_mainMenu.h"
#include "screen_analyze.h"
#include "screen_setting.h"
#include "screen_settingKeys.h"
#include "screen_gameType.h"
#include "screen_choiceArena.h"
#include "screen_table.h"
#include "screen_credits.h"
#include "screen_browser.h"

static void initGame()
{
	createHomeDirector();

	initSDL();

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
#ifdef DEBUG
	printf(_("Quitting TUXANCI...\n"));
#endif
	exit(0);
}

void startGame()
{
	initGame();

	startScreen("mainMenu");

	eventSDL();
	quitGame();
}
