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
#include "mouse_buffer.h"

#ifndef NO_SOUND
#include "audio.h"
#include "sound.h"
#include "music.h"
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
	home_director_create();

	mouse_buffer_init();
	interface_init();
	font_init();
	layer_init();
	image_init();
#ifndef NO_SOUND
	audio_init();
	sound_init();
	music_init();
#endif /* NO_SOUND */
	screen_init();
	arena_file_init();
	tux_init();
	item_init();
	shot_init();
	panel_init();
	world_init();

	main_menu_init();
	analyze_init();
	choice_arena_init();
	setting_init();
	setting_key_int();
	game_type_init();
	down_arena_init();
	scredits_init();
	table_init();
	browser_init();
}

void game_quit()
{
	mouse_buffer_quit();
	interface_quit();

	main_menu_quit();
	analyze_quit();
	setting_quit();
	setting_key_quit();
	game_type_quit();
	down_arena_quit();
	choice_arena_quit();
	scredits_quit();
	table_quit();
	browser_quit();

	layer_quit();
	font_quit();
	screen_quit();
	arena_file_quit();
	item_quiy();
	tux_quit();
	shot_quit();
	panel_quit();
	world_quit();
	image_quit();
#ifndef NO_SOUND
	sound_quit();
	music_quit();
	audio_quit();
#endif /* NO_SOUND */

	DEBUG_MSG(_("[Debug] Shutting down the game\n"));

	exit(0);
}

void game_start()
{
	initGame();

	screen_start("mainMenu");

	interface_event();
	game_quit();
}
