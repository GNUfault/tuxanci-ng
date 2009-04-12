
#include <stdio.h>
#include <assert.h>

#include "main.h"
#include "list.h"
#include "tux.h"
#include "item.h"
#include "shot.h"
#include "gun.h"
#include "myTimer.h"
#include "net_multiplayer.h"
#include "arena.h"
#include "arenaFile.h"
#include "proto.h"
#include "modules.h"
#include "idManager.h"
#include "serverSendMsg.h"

#include "interface.h"
#include "layer.h"
#include "image.h"
#include "font.h"

#include "hotKey.h"
#include "panel.h"
#include "radar.h"
#include "chat.h"
#include "pauza.h"
#include "term.h"
#include "saveDialog.h"
#include "saveLoad.h"

#ifndef NO_SOUND
#    include "music.h"
#    include "sound.h"
#endif

#include "screen.h"
#include "world.h"
#include "analyze.h"
#include "setting.h"
#include "gameType.h"
#include "settingKeys.h"
#include "choiceArena.h"
#include "table.h"


static arena_t *arena;
static bool_t isScreenWorldInit = FALSE;
static bool_t isEndWorld;

static tux_t *tuxWithControlRightKeyboard;
static tux_t *tuxWithControlLeftKeyboard;

bool_t world_is_inicialized()
{
	return isScreenWorldInit;
}

void setGameType()
{
	int ret = 0;

	ret = net_multiplayer_init(public_server_get_settingGameType(), public_server_get_settingIP(),
				public_server_get_settingPort(), public_server_get_settingProto());

	if (ret != 0) {
		fprintf(stderr, _("Network initialization error!\n"));
		world_do_end();
	}
}

void world_set_arena(arenaFile_t * arenaFile)
{
	arena = arena_file_get_arena(arenaFile);

#ifndef NO_SOUND
	music_play(arena->music, MUSIC_GROUP_USER);
#endif
}

void world_do_end()
{
	isEndWorld = TRUE;
}

static void timer_endArena()
{
	if (net_multiplayer_get_game_type() != NET_GAME_TYPE_CLIENT) {
		world_do_end();
		return;
	}
}

void world_inc_round()
{
	if (arena->max_countRound == WORLD_COUNT_ROUND_UNLIMITED ||
		arena->countRound >= arena->max_countRound) {
		return;
	}

	arena->countRound++;
	//printf("count %d/%d\n", count, max_count);

	if (arena->countRound >= arena->max_countRound) {
		//printf("count %d ending\n", count);
#ifndef NO_SOUND
		sound_play("end", SOUND_GROUP_BASE);
#endif
		timer_add_task(arena_get_current()->listTimer, TIMER_ONE, timer_endArena, NULL, TIMER_END_ARENA);
	}
}

void prepareArena()
{
	tux_t *tux;
	char name[STR_NAME_SIZE];

	//printf("public_server_get_settingAI = %s\n", public_server_get_settingAI());
	arena_set_current(NULL);
	tuxWithControlRightKeyboard = NULL;
	tuxWithControlLeftKeyboard = NULL;

	if (game_type_load_session() != NULL) {
		load_arena(game_type_load_session());
		return;
	}

	switch (net_multiplayer_get_game_type()) {
	case NET_GAME_TYPE_NONE:
		world_set_arena(choice_arena_get());
		item_add_new_item(arena->spaceItem, ID_UNKNOWN);
		public_server_get_settingCountRound(&arena->max_countRound);

		tux = tux_new();
		tux->control = TUX_CONTROL_KEYBOARD_RIGHT;
		tuxWithControlRightKeyboard = tux;
		public_server_get_setting_name_right(name);
		tux_set_name(tux, name);
		space_add(arena->spaceTux, tux);

		tux = tux_new();
		tux->control = TUX_CONTROL_KEYBOARD_LEFT;
		tuxWithControlLeftKeyboard = tux;
		public_server_get_settingNameLeft(name);
		tux_set_name(tux, name);
		space_add(arena->spaceTux, tux);

		if (setting_is_ai()) {
			tux->control = TUX_CONTROL_AI;

			if (module_load("libmodAI") != 0) {
				tux->control = TUX_CONTROL_KEYBOARD_LEFT;
			}
		}
		//printf("game_type_load_session = %s\n", game_type_load_session());

		break;

	case NET_GAME_TYPE_SERVER:
		world_set_arena(choice_arena_get());
		item_add_new_item(arena->spaceItem, ID_UNKNOWN);
		public_server_get_settingCountRound(&arena->max_countRound);

		tux = tux_new();
		tux->control = TUX_CONTROL_KEYBOARD_RIGHT;
		tuxWithControlRightKeyboard = tux;
		public_server_get_setting_name_right(name);
		tux_set_name(tux, name);
		space_add(arena->spaceTux, tux);
		break;

	case NET_GAME_TYPE_CLIENT:
		break;
	}
}

void world_draw()
{
	if (arena != NULL) {
		arena_draw(arena);
		panel_draw(tuxWithControlRightKeyboard, tuxWithControlLeftKeyboard);

		if (arena->w > WINDOW_SIZE_X || arena->h > WINDOW_SIZE_Y) {
			radar_draw(arena);
		}
	}

	chat_draw();
	pauza_draw();
	term_draw();
	save_dialog_draw();
}

static void netAction(tux_t * tux, int action)
{
	if (public_server_get_settingGameType() == NET_GAME_TYPE_SERVER) {
		proto_send_event_server(PROTO_SEND_ALL_SEES_TUX, NULL, tux, action);
	}

	if (public_server_get_settingGameType() == NET_GAME_TYPE_CLIENT) {
		proto_send_event_client(action);
	}
}

static void control_keyboard_right(tux_t * tux)
{
	static int lastKey = 0;
	int countKey;
	Uint8 *mapa;
	mapa = SDL_GetKeyState(NULL);

	assert(tux != NULL);
	assert(mapa != NULL);

	countKey = 0;

	if (mapa[(SDLKey) key_table_get_key(KEY_TUX_RIGHT_MOVE_UP)] == SDL_PRESSED)
		countKey++;

	if (mapa[(SDLKey) key_table_get_key(KEY_TUX_RIGHT_MOVE_RIGHT)] == SDL_PRESSED)
		countKey++;

	if (mapa[(SDLKey) key_table_get_key(KEY_TUX_RIGHT_MOVE_LEFT)] == SDL_PRESSED)
		countKey++;

	if (mapa[(SDLKey) key_table_get_key(KEY_TUX_RIGHT_MOVE_DOWN)] == SDL_PRESSED)
		countKey++;

	if (mapa[(SDLKey) key_table_get_key(KEY_TUX_RIGHT_SWITCH_WEAPON)] == SDL_PRESSED
		&& tux->isCanSwitchGun == TRUE) {
		netAction(tux, TUX_SWITCH_GUN);
		tux_action(tux, TUX_SWITCH_GUN);
		return;
	}

	if (mapa[(SDLKey) key_table_get_key(KEY_TUX_RIGHT_SHOOT)] == SDL_PRESSED
		&& tux->isCanShot == TRUE) {
		netAction(tux, TUX_SHOT);
		tux_action(tux, TUX_SHOT);
		return;
	}

	if (mapa[(SDLKey) key_table_get_key(KEY_TUX_RIGHT_MOVE_UP)] == SDL_PRESSED) {
		if (countKey > 1 && lastKey == key_table_get_key(KEY_TUX_RIGHT_MOVE_UP))
			goto tuUp;

		if (countKey == 1)
			lastKey = key_table_get_key(KEY_TUX_RIGHT_MOVE_UP);
		netAction(tux, TUX_UP);
		tux_action(tux, TUX_UP);
		return;

	  tuUp:;
	}

	if (mapa[(SDLKey) key_table_get_key(KEY_TUX_RIGHT_MOVE_RIGHT)] == SDL_PRESSED) {
		if (countKey > 1 && lastKey == key_table_get_key(KEY_TUX_RIGHT_MOVE_RIGHT))
			goto tuRight;

		if (countKey == 1)
			lastKey = key_table_get_key(KEY_TUX_RIGHT_MOVE_RIGHT);
		netAction(tux, TUX_RIGHT);
		tux_action(tux, TUX_RIGHT);
		return;

	  tuRight:;

	}

	if (mapa[(SDLKey) key_table_get_key(KEY_TUX_RIGHT_MOVE_LEFT)] == SDL_PRESSED) {
		if (countKey > 1 && lastKey == key_table_get_key(KEY_TUX_RIGHT_MOVE_LEFT))
			goto tuLeft;

		if (countKey == 1)
			lastKey = key_table_get_key(KEY_TUX_RIGHT_MOVE_LEFT);
		netAction(tux, TUX_LEFT);
		tux_action(tux, TUX_LEFT);
		return;

	  tuLeft:;
	}

	if (mapa[(SDLKey) key_table_get_key(KEY_TUX_RIGHT_MOVE_DOWN)] == SDL_PRESSED) {
		if (countKey > 1 && lastKey == key_table_get_key(KEY_TUX_RIGHT_MOVE_DOWN))
			goto tuDown;

		if (countKey == 1)
			lastKey = key_table_get_key(KEY_TUX_RIGHT_MOVE_DOWN);
		netAction(tux, TUX_DOWN);
		tux_action(tux, TUX_DOWN);
		return;

	  tuDown:;
	}
}

static void control_keyboard_left(tux_t * tux)
{
	static int lastKey = 0;
	int countKey;
	Uint8 *mapa;
	mapa = SDL_GetKeyState(NULL);

	assert(tux != NULL);
	assert(mapa != NULL);

	countKey = 0;

	if (mapa[(SDLKey) key_table_get_key(KEY_TUX_LEFT_MOVE_UP)] == SDL_PRESSED)
		countKey++;

	if (mapa[(SDLKey) key_table_get_key(KEY_TUX_LEFT_MOVE_RIGHT)] == SDL_PRESSED)
		countKey++;

	if (mapa[(SDLKey) key_table_get_key(KEY_TUX_LEFT_MOVE_LEFT)] == SDL_PRESSED)
		countKey++;

	if (mapa[(SDLKey) key_table_get_key(KEY_TUX_LEFT_MOVE_DOWN)] == SDL_PRESSED)
		countKey++;

	if (mapa[(SDLKey) key_table_get_key(KEY_TUX_LEFT_SWITCH_WEAPON)] == SDL_PRESSED) {
		if (tux->isCanSwitchGun == TRUE) {
			tux_action(tux, TUX_SWITCH_GUN);
			return;
		}
	}

	if (mapa[(SDLKey) key_table_get_key(KEY_TUX_LEFT_SHOOT)] == SDL_PRESSED
		&& tux->isCanShot == TRUE) {
		tux_action(tux, TUX_SHOT);
		return;
	}

	if (mapa[(SDLKey) key_table_get_key(KEY_TUX_LEFT_MOVE_UP)] == SDL_PRESSED) {
		if (countKey > 1 && lastKey == key_table_get_key(KEY_TUX_LEFT_MOVE_UP))
			goto tuUp;

		if (countKey == 1)
			lastKey = key_table_get_key(KEY_TUX_LEFT_MOVE_UP);
		tux_action(tux, TUX_UP);
		return;

	  tuUp:;
	}

	if (mapa[(SDLKey) key_table_get_key(KEY_TUX_LEFT_MOVE_RIGHT)] == SDL_PRESSED) {
		if (countKey > 1 && lastKey == key_table_get_key(KEY_TUX_LEFT_MOVE_RIGHT))
			goto tuRight;

		if (countKey == 1)
			lastKey = key_table_get_key(KEY_TUX_LEFT_MOVE_RIGHT);
		tux_action(tux, TUX_RIGHT);
		return;

	  tuRight:;

	}

	if (mapa[(SDLKey) key_table_get_key(KEY_TUX_LEFT_MOVE_LEFT)] == SDL_PRESSED) {
		if (countKey > 1 && lastKey == key_table_get_key(KEY_TUX_LEFT_MOVE_LEFT))
			goto tuLeft;

		if (countKey == 1)
			lastKey = key_table_get_key(KEY_TUX_LEFT_MOVE_LEFT);
		tux_action(tux, TUX_LEFT);
		return;

	  tuLeft:;
	}

	if (mapa[(SDLKey) key_table_get_key(KEY_TUX_LEFT_MOVE_DOWN)] == SDL_PRESSED) {
		if (countKey > 1 && lastKey == key_table_get_key(KEY_TUX_LEFT_MOVE_DOWN))
			goto tuDown;

		if (countKey == 1)
			lastKey = key_table_get_key(KEY_TUX_LEFT_MOVE_DOWN);
		tux_action(tux, TUX_DOWN);
		return;

	  tuDown:;
	}
}

static void eventEnd()
{
	if (isEndWorld == TRUE) {
		screen_set("analyze");
		return;
	}
}

tux_t *world_get_control_tux(int control_type)
{
	switch (control_type) {
		case TUX_CONTROL_KEYBOARD_RIGHT:
			return tuxWithControlRightKeyboard;
			break;
		case TUX_CONTROL_KEYBOARD_LEFT:
			return tuxWithControlLeftKeyboard;
			break;
	}

	return NULL;
}

void world_set_control_tux(tux_t * tux, int control_type)
{
	switch (control_type) {
		case TUX_CONTROL_KEYBOARD_RIGHT:
			tuxWithControlRightKeyboard = tux;
			break;
		case TUX_CONTROL_KEYBOARD_LEFT:
			tuxWithControlLeftKeyboard = tux;
			break;
	}
}

void world_tux_control(tux_t * p)
{
	assert(p != NULL);

	if (p->status != TUX_STATUS_ALIVE) {
		return;
	}

	switch (p->control) {
		case TUX_CONTROL_NONE:
			assert(!_("Tux has not defined controls!"));
			break;
	
		case TUX_CONTROL_KEYBOARD_LEFT:
			if (chat_is_active() == FALSE) {
				control_keyboard_left(p);
			}
			break;
	
		case TUX_CONTROL_KEYBOARD_RIGHT:
			if (chat_is_active() == FALSE) {
				control_keyboard_right(p);
			}
			break;
	}
}

void world_event()
{
	tux_t *thisTux;

	if (arena == NULL) {
		net_multiplayer_event();
		eventEnd();
		return;
	}

	net_multiplayer_event();

	if (pauza_is_active() == FALSE && save_dialog_is_active() == FALSE) {
		arena_event(arena);
		//module_event();
	}

	thisTux = world_get_control_tux(TUX_CONTROL_KEYBOARD_RIGHT);

	radar_add(thisTux->id, thisTux->x, thisTux->y, RADAR_TYPE_YOU);

	thisTux = world_get_control_tux(TUX_CONTROL_KEYBOARD_LEFT);

	if (thisTux != NULL) {
		radar_add(thisTux->id, thisTux->x, thisTux->y, RADAR_TYPE_TUX);
	}

	eventEnd();
	chat_event();
	pauza_event();
	term_event();
	save_dialog_event();

}

static void hotkey_escape()
{
	world_do_end();
}

void startWorld()
{
	arena = NULL;
	isEndWorld = FALSE;

	hot_key_register(SDLK_ESCAPE, hotkey_escape);
	arena_init();
	id_init_list();
	radar_init();
	pauza_init();
	term_init();
	save_dialog_init();

	module_init();
	setGameType();
	chat_init();
	prepareArena();
}

static void action_analyze(space_t * space, tux_t * tux, void *p)
{
	analyze_add(tux->name, tux->score);
}

static void setAnalyze()
{
	analyze_restart();

	space_action(arena->spaceTux, action_analyze, NULL);

	analyze_end();
}

static void setTable()
{
	tux_t *tuxRight;
	tux_t *tuxLeft;

	if (public_server_get_settingGameType() != NET_GAME_TYPE_NONE) {
		return;
	}

	tuxRight = world_get_control_tux(TUX_CONTROL_KEYBOARD_RIGHT);
	tuxLeft = world_get_control_tux(TUX_CONTROL_KEYBOARD_LEFT);

	if (tuxRight->score > tuxLeft->score) {
		table_add(tuxRight->name, tuxRight->score);
	}

	if (tuxLeft->score > tuxRight->score) {
		table_add(tuxLeft->name, tuxLeft->score);
	}
}

void stoptWorld()
{
	if (arena != NULL) {
		setTable();
		setAnalyze();
	}

	hot_key_unregister(SDLK_ESCAPE);
	arena_quit();

	net_multiplayer_quit();

	if (arena != NULL) {
		arena_destroy(arena);
	}

	radar_quit();
	pauza_quit();
	term_quit();
	save_dialog_quit();


#ifndef NO_SOUND
	music_stop();
#endif
	image_del_all_image_in_group(IMAGE_GROUP_USER);

#ifndef NO_SOUND
	music_del_all_in_group(MUSIC_GROUP_USER);
#endif
	module_quit();
	chat_quit();
	id_quit_list();
}

void world_init()
{
	assert(image_is_inicialized() == TRUE);
	assert(tux_is_inicialized() == TRUE);
	assert(shot_is_inicialized() == TRUE);
	assert(panel_is_inicialized() == TRUE);
	assert(screen_is_inicialized() == TRUE);

	screen_register(screen_new
				   ("world", startWorld, world_event, world_draw, stoptWorld));

#ifndef NO_SOUND
	sound_add("dead.ogg", "dead", SOUND_GROUP_BASE);
	sound_add("explozion.ogg", "explozion", SOUND_GROUP_BASE);
	sound_add("gun_lasser.ogg", "gun_lasser", SOUND_GROUP_BASE);
	sound_add("gun_revolver.ogg", "gun_revolver", SOUND_GROUP_BASE);
	sound_add("gun_scatter.ogg", "gun_scatter", SOUND_GROUP_BASE);
	sound_add("gun_tommy.ogg", "gun_tommy", SOUND_GROUP_BASE);
	sound_add("put_mine.ogg", "put_mine", SOUND_GROUP_BASE);
	sound_add("teleport.ogg", "teleport", SOUND_GROUP_BASE);
	sound_add("item_bonus.ogg", "item_bonus", SOUND_GROUP_BASE);
	sound_add("item_gun.ogg", "item_gun", SOUND_GROUP_BASE);
	sound_add("switch_gun.ogg", "switch_gun", SOUND_GROUP_BASE);
	sound_add("end.ogg", "end", SOUND_GROUP_BASE);
#endif

	isScreenWorldInit = TRUE;
}

void world_quit()
{
	DEBUG_MSG(_("Quitting screen world\n"));
	isScreenWorldInit = FALSE;
}
