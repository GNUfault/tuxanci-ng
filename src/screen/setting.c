
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "main.h"
#include "list.h"
#include "tux.h"
#include "textFile.h"
#include "director.h"
#include "homeDirector.h"
#include "arenaFile.h"

#include "configFile.h"
#include "interface.h"
#include "screen.h"
#include "image.h"
#include "hotKey.h"

#ifndef NO_SOUND
#    include "music.h"
#    include "sound.h"
#endif

#include "mainMenu.h"
#include "setting.h"
#include "settingKeys.h"
#include "settingKeys.h"
#include "choiceArena.h"

#include "widget.h"
#include "widget_label.h"
#include "widget_button.h"
#include "widget_image.h"
#include "widget_textfield.h"
#include "widget_check.h"
#include "widget_select.h"

static widget_t *image_backgorund;

static widget_t *button_back;
static widget_t *button_keys;	// setting of keycontrols

static widget_t *label_count_round;
static widget_t *label_name_player1;
static widget_t *label_name_player2;
static widget_t *label_ai;

#ifndef NO_SOUND
static widget_t *label_music;
static widget_t *label_sound;
#endif

static widget_t *check[ITEM_COUNT];
#ifndef NO_SOUND
static widget_t *check_music;
static widget_t *check_sound;
#endif

static widget_t *image_gun_dual_revolver;
static widget_t *image_gun_scatter;
static widget_t *image_gun_tommy;
static widget_t *image_gun_lasser;
static widget_t *image_gun_mine;
static widget_t *image_gun_bombball;

static widget_t *image_bonus_speed;
static widget_t *image_bonus_shot;
static widget_t *image_bonus_teleport;
static widget_t *image_bonus_ghost;
static widget_t *image_bonus_4x;
static widget_t *image_bonus_hidden;

static widget_t *textfield_count_cound;
static widget_t *textfield_name_player1;
static widget_t *textfield_name_player2;

static widget_t *check_ai;

static textFile_t *configFile;

static void hotkey_escape()
{
	screen_set("mainMenu");
}

void screen_startSetting()
{
#ifndef NO_SOUND
	music_play("menu", MUSIC_GROUP_BASE);
#endif

	hot_key_register(SDLK_ESCAPE, hotkey_escape);
}

void setting_draw()
{
	int i;

	wid_image_draw(image_backgorund);

	label_draw(label_count_round);
	label_draw(label_name_player1);
	label_draw(label_ai);

#ifndef NO_SOUND
	label_draw(label_music);
	label_draw(label_sound);
#endif

	text_field_draw(textfield_count_cound);
	text_field_draw(textfield_name_player1);

	if (check_get_status(check_ai) == FALSE) {
		label_draw(label_name_player2);
		text_field_draw(textfield_name_player2);
	}

	wid_image_draw(image_gun_dual_revolver);
	wid_image_draw(image_gun_scatter);
	wid_image_draw(image_gun_tommy);
	wid_image_draw(image_gun_lasser);
	wid_image_draw(image_gun_mine);
	wid_image_draw(image_gun_bombball);

	wid_image_draw(image_bonus_speed);
	wid_image_draw(image_bonus_shot);
	wid_image_draw(image_bonus_teleport);
	wid_image_draw(image_bonus_ghost);
	wid_image_draw(image_bonus_4x);
	wid_image_draw(image_bonus_hidden);

#ifndef NO_SOUND
	check_draw(check_music);
	check_draw(check_sound);
#endif

	for (i = GUN_DUAL_SIMPLE; i <= GUN_BOMBBALL; i++) {
		check_draw(check[i]);
	}

	for (i = BONUS_SPEED; i <= BONUS_HIDDEN; i++) {
		check_draw(check[i]);
	}

	check_draw(check_ai);

	button_draw(button_back);
	button_draw(button_keys);
}

void setting_event()
{
	int i;

	text_field_event(textfield_count_cound);
	text_field_event(textfield_name_player1);

	if (check_get_status(check_ai) == FALSE) {
		text_field_event(textfield_name_player2);
	}

#ifndef NO_SOUND
	check_event(check_music);
	check_event(check_sound);
#endif

	for (i = GUN_DUAL_SIMPLE; i <= GUN_BOMBBALL; i++) {
		check_event(check[i]);
	}

	for (i = BONUS_SPEED; i <= BONUS_HIDDEN; i++) {
		check_event(check[i]);
	}

	check_event(check_ai);

	button_event(button_back);
	button_event(button_keys);
}

void stopScreenSetting()
{
	hot_key_unregister(SDLK_ESCAPE);
}

static void eventWidget(void *p)
{
	widget_t *my_button;
	widget_t *my_check;

	my_button = (widget_t *) (p);
	my_check = (widget_t *) (p);

	if (my_button == button_back) {
		screen_set("mainMenu");
	}

	if (my_button == button_keys) {
		screen_set("settingKeys");
	}

#ifndef NO_SOUND
	if (my_check == check_music) {
		music_set_active(check_get_status(check_music));
	}

	if (my_check == check_sound) {
		sound_set_active(check_get_status(check_sound));
	}
#endif
}

static void initSettingFile()
{
	char path[STR_PATH_SIZE];
	char val[STR_SIZE];

	sprintf(path, "%s/tuxanci.conf", home_director_get());

	configFile = text_file_load(path);

	if (configFile == NULL) {
		fprintf(stderr, _("I am unable to load: \"%s\"!\n"), path);
		fprintf(stderr, _("Creating: \"%s\"\n"), path);

		configFile = text_file_new(path);
	}

	if (configFile == NULL) {
		fprintf(stderr, _("I was unable to create: \"%s\"!\n"), path);
		return;
	}

	loadValueFromConfigFile(configFile, "COUNT_ROUND", val, STR_SIZE, "15");
	text_field_set_text(textfield_count_cound, val);

	loadValueFromConfigFile(configFile, "NAME_PLAYER_RIGHT", val, STR_SIZE,
							NAME_PLAYER_RIGHT);
	text_field_set_text(textfield_name_player1, val);

	loadValueFromConfigFile(configFile, "NAME_PLAYER_LEFT", val, STR_SIZE,
							NAME_PLAYER_LEFT);
	text_field_set_text(textfield_name_player2, val);

	loadValueFromConfigFile(configFile, "GUN_DUAL_SIMPLE", val, STR_SIZE,
							"YES");
	check_set_status(check[GUN_DUAL_SIMPLE], isYesOrNO(val));
	loadValueFromConfigFile(configFile, "GUN_SCATTER", val, STR_SIZE, "YES");
	check_set_status(check[GUN_SCATTER], isYesOrNO(val));
	loadValueFromConfigFile(configFile, "GUN_TOMMY", val, STR_SIZE, "YES");
	check_set_status(check[GUN_TOMMY], isYesOrNO(val));
	loadValueFromConfigFile(configFile, "GUN_LASSER", val, STR_SIZE, "YES");
	check_set_status(check[GUN_LASSER], isYesOrNO(val));
	loadValueFromConfigFile(configFile, "GUN_MINE", val, STR_SIZE, "YES");
	check_set_status(check[GUN_MINE], isYesOrNO(val));
	loadValueFromConfigFile(configFile, "GUN_BOMBBALL", val, STR_SIZE, "YES");
	check_set_status(check[GUN_BOMBBALL], isYesOrNO(val));

	loadValueFromConfigFile(configFile, "BONUS_SPEED", val, STR_SIZE, "YES");
	check_set_status(check[BONUS_SPEED], isYesOrNO(val));
	loadValueFromConfigFile(configFile, "BONUS_SHOT", val, STR_SIZE, "YES");
	check_set_status(check[BONUS_SHOT], isYesOrNO(val));
	loadValueFromConfigFile(configFile, "BONUS_TELEPORT", val, STR_SIZE,
							"YES");
	check_set_status(check[BONUS_TELEPORT], isYesOrNO(val));
	loadValueFromConfigFile(configFile, "BONUS_GHOST", val, STR_SIZE, "YES");
	check_set_status(check[BONUS_GHOST], isYesOrNO(val));
	loadValueFromConfigFile(configFile, "BONUS_4X", val, STR_SIZE, "YES");
	check_set_status(check[BONUS_4X], isYesOrNO(val));
	loadValueFromConfigFile(configFile, "BONUS_HIDDEN", val, STR_SIZE, "YES");
	check_set_status(check[BONUS_HIDDEN], isYesOrNO(val));

	loadValueFromConfigFile(configFile, "AI", val, STR_SIZE, "NO");
	check_set_status(check_ai, isYesOrNO(val));

#ifndef NO_SOUND
	loadValueFromConfigFile(configFile, "MUSIC", val, STR_SIZE, "YES");
	check_set_status(check_music, isYesOrNO(val));

	loadValueFromConfigFile(configFile, "SOUND", val, STR_SIZE, "YES");
	check_set_status(check_sound, isYesOrNO(val));
#endif

	loadValueFromConfigFile(configFile, "ARENA", val, STR_SIZE, "FAGN");
	choice_arena_set(arena_file_get_file_format_net_name(val));

	text_file_save(configFile);
}

static void saveAndDestroyConfigFile()
{
	if (configFile == NULL) {
		fprintf(stderr, _("I am unable to save configure file, because config file was not initialised!\n"));
		return;
	}

	setValueInConfigFile(configFile, "COUNT_ROUND",  text_field_get_text(textfield_count_cound));
	setValueInConfigFile(configFile, "NAME_PLAYER_RIGHT", text_field_get_text(textfield_name_player1));
	setValueInConfigFile(configFile, "NAME_PLAYER_LEFT", text_field_get_text(textfield_name_player2));
	setValueInConfigFile(configFile, "GUN_DUAL_SIMPLE", getYesOrNo(check_get_status(check[GUN_DUAL_SIMPLE])));
	setValueInConfigFile(configFile, "GUN_SCATTER", getYesOrNo(check_get_status(check[GUN_SCATTER])));
	setValueInConfigFile(configFile, "GUN_TOMMY", getYesOrNo(check_get_status(check[GUN_TOMMY])));
	setValueInConfigFile(configFile, "GUN_LASSER", getYesOrNo(check_get_status(check[GUN_LASSER])));
	setValueInConfigFile(configFile, "GUN_MINE", getYesOrNo(check_get_status(check[GUN_MINE])));
	setValueInConfigFile(configFile, "GUN_BOMBBALL", getYesOrNo(check_get_status(check[GUN_BOMBBALL])));
	setValueInConfigFile(configFile, "BONUS_SPEED", getYesOrNo(check_get_status(check[BONUS_SPEED])));
	setValueInConfigFile(configFile, "BONUS_SHOT", getYesOrNo(check_get_status(check[BONUS_SHOT])));
	setValueInConfigFile(configFile, "BONUS_TELEPORT", getYesOrNo(check_get_status (check[BONUS_TELEPORT])));
	setValueInConfigFile(configFile, "BONUS_GHOST", getYesOrNo(check_get_status(check[BONUS_GHOST])));
	setValueInConfigFile(configFile, "BONUS_4X", getYesOrNo(check_get_status(check[BONUS_4X])));
	setValueInConfigFile(configFile, "BONUS_HIDDEN", getYesOrNo(check_get_status(check[BONUS_HIDDEN])));
	setValueInConfigFile(configFile, "AI", getYesOrNo(check_get_status(check_ai)));

#ifndef NO_SOUND
	setValueInConfigFile(configFile, "MUSIC", getYesOrNo(check_get_status(check_music)));
	setValueInConfigFile(configFile, "SOUND", getYesOrNo(check_get_status(check_sound)));
#endif

	if( choice_arena_get() != NULL )
	{
		setValueInConfigFile(configFile, "ARENA", arena_file_get_net_name(choice_arena_get()));
	}
	//TODO

	text_file_save(configFile);
	text_file_destroy(configFile);
}

void setting_init()
{
	image_t *image;
	int i;

	image = image_get(IMAGE_GROUP_BASE, "screen_main");
	image_backgorund = wid_image_new(0, 0, image);

	button_back = button_new(_("back"), WINDOW_SIZE_X - 200, WINDOW_SIZE_Y - 100, eventWidget);

	button_keys = button_new(_("controls"), WINDOW_SIZE_X - 200,
				      button_back->y - WIDGET_BUTTON_HEIGHT - 10, eventWidget);

#ifndef NO_SOUND
	label_music = label_new(_("Music:"), 100, WINDOW_SIZE_Y - 85, WIDGET_LABEL_LEFT);

	check_music = check_new(label_music->x + label_music->w + 10,
				     WINDOW_SIZE_Y - 80, music_is_active(), eventWidget);
	label_sound = label_new(_("Sound:"), check_music->x + WIDGET_CHECK_WIDTH + 10,
				     WINDOW_SIZE_Y - 85, WIDGET_LABEL_LEFT);

	check_sound = check_new(label_sound->x + label_sound->w + 10,
				     WINDOW_SIZE_Y - 80, sound_is_active(), eventWidget);

	label_ai = label_new(_("AI:"), check_sound->x + WIDGET_CHECK_WIDTH + 10,
			 	  WINDOW_SIZE_Y - 85, WIDGET_LABEL_LEFT);
#else
	label_ai = label_new(_("AI:"), 100, WINDOW_SIZE_Y - 85, WIDGET_LABEL_LEFT);
#endif

	check_ai = check_new(label_ai->x + label_ai->w + 10, WINDOW_SIZE_Y - 80, FALSE, eventWidget);

	label_count_round = label_new(_("No. of rounds:"), 100, WINDOW_SIZE_Y - 200, WIDGET_LABEL_LEFT);
	label_name_player1 = label_new(_("Player 1:"), 100, WINDOW_SIZE_Y - 160, WIDGET_LABEL_LEFT);
	label_name_player2 = label_new(_("Player 2:"), 100, WINDOW_SIZE_Y - 120, WIDGET_LABEL_LEFT);

	textfield_count_cound = text_field_new(getParamElse("--count", "15"),
				WIDGET_TEXTFIELD_FILTER_NUM,
				110 + label_count_round->w,
				 WINDOW_SIZE_Y - 200);

	textfield_name_player1 = text_field_new(getParamElse("--name1", NAME_PLAYER_RIGHT),
				WIDGET_TEXTFIELD_FILTER_ALPHANUM,
				110 + label_count_round->w, WINDOW_SIZE_Y - 160);

	textfield_name_player2 = text_field_new(getParamElse("--name2", NAME_PLAYER_LEFT),
				WIDGET_TEXTFIELD_FILTER_ALPHANUM,
				110 + label_count_round->w, WINDOW_SIZE_Y - 120);

	for (i = GUN_DUAL_SIMPLE; i <= GUN_BOMBBALL; i++) {
		int x = 0;
		int y = 0;

		if (i >= GUN_DUAL_SIMPLE && i <= GUN_TOMMY) {
			x = 100;
			y = 200 + (i - GUN_DUAL_SIMPLE) * 50;
		}

		if (i >= GUN_LASSER && i <= GUN_BOMBBALL) {
			x = 250;
			y = 200 + (i - GUN_LASSER) * 50;
		}

		check[i] = check_new(x, y, TRUE, NULL);
	}

	for (i = BONUS_SPEED; i <= BONUS_HIDDEN; i++) {
		int x = 0;
		int y = 0;

		if (i >= BONUS_SPEED && i <= BONUS_TELEPORT) {
			x = 430;
			y = 200 + (i - BONUS_SPEED) * 50;
		}

		if (i >= BONUS_GHOST && i <= BONUS_HIDDEN) {
			x = 580;
			y = 200 + (i - BONUS_GHOST) * 50;
		}

		check[i] = check_new(x, y, TRUE, NULL);
	}

	image_gun_dual_revolver = wid_image_new(110 + WIDGET_CHECK_WIDTH, 200, image_get(IMAGE_GROUP_BASE, "panel_dual"));
	image_gun_scatter = wid_image_new(110 + WIDGET_CHECK_WIDTH, 250, image_get(IMAGE_GROUP_BASE, "panel_scatter"));
	image_gun_tommy = wid_image_new(110 + WIDGET_CHECK_WIDTH, 300, image_get(IMAGE_GROUP_BASE, "panel_tommy"));
	image_gun_lasser = wid_image_new(260 + WIDGET_CHECK_WIDTH, 200,image_get(IMAGE_GROUP_BASE, "panel_lasser"));
	image_gun_mine = wid_image_new(260 + WIDGET_CHECK_WIDTH, 250, image_get(IMAGE_GROUP_BASE, "panel_mine"));
	image_gun_bombball = wid_image_new(260 + WIDGET_CHECK_WIDTH, 300, image_get(IMAGE_GROUP_BASE, "panel_bombball"));
	image_bonus_speed = wid_image_new(430 + WIDGET_CHECK_WIDTH, 200, image_get(IMAGE_GROUP_BASE, "panel_speed"));;
	image_bonus_shot = wid_image_new(430 + WIDGET_CHECK_WIDTH, 250, image_get(IMAGE_GROUP_BASE, "panel_shot"));;
	image_bonus_teleport = wid_image_new(430 + WIDGET_CHECK_WIDTH, 300, image_get(IMAGE_GROUP_BASE, "panel_teleport"));;
	image_bonus_ghost = wid_image_new(580 + WIDGET_CHECK_WIDTH, 200, image_get(IMAGE_GROUP_BASE, "panel_ghost"));;
	image_bonus_4x = wid_image_new(580 + WIDGET_CHECK_WIDTH, 250, image_get(IMAGE_GROUP_BASE, "panel_4x"));;
	image_bonus_hidden = wid_image_new(580 + WIDGET_CHECK_WIDTH, 300, image_get(IMAGE_GROUP_BASE, "panel_hidden"));;

	screen_register( screen_new("setting", screen_startSetting, setting_event,
			setting_draw, stopScreenSetting));

	initSettingFile();

#ifndef NO_SOUND
	eventWidget(check_music);
	eventWidget(check_sound);
#endif
}

void public_server_get_setting_name_right(char *s)
{
	strcpy(s, text_field_get_text(textfield_name_player1));
}

void public_server_get_settingNameLeft(char *s)
{
	if (check_get_status(check_ai)) {
		strcpy(s, NAME_AI);
	} else {
		strcpy(s, text_field_get_text(textfield_name_player2));
	}
}

void public_server_get_settingCountRound(int *n)
{
	*n = atoi(text_field_get_text(textfield_count_cound));
}

bool_t setting_is_ai()
{
	return check_get_status(check_ai);
}

bool_t setting_is_any_item()
{
	int i;

	for (i = GUN_DUAL_SIMPLE; i <= GUN_BOMBBALL; i++) {
		if (check_get_status(check[i]) == TRUE) {
			return TRUE;
		}
	}

	for (i = BONUS_SPEED; i < BONUS_HIDDEN; i++) {
		if (check_get_status(check[i]) == TRUE) {
			return TRUE;
		}
	}

	return FALSE;
}

bool_t setting_is_item(int item)
{
	return check_get_status(check[item]);
}

void setting_quit()
{
	int i;

	saveAndDestroyConfigFile();

	wid_image_destroy(image_backgorund);

	label_destroy(label_count_round);
	label_destroy(label_name_player1);
	label_destroy(label_ai);

#ifndef NO_SOUND
	label_destroy(label_music);
	label_destroy(label_sound);
#endif
	label_destroy(label_name_player2);
	text_field_destroy(textfield_name_player2);

	text_field_destroy(textfield_name_player1);
	text_field_destroy(textfield_count_cound);

	wid_image_destroy(image_gun_dual_revolver);
	wid_image_destroy(image_gun_scatter);
	wid_image_destroy(image_gun_tommy);
	wid_image_destroy(image_gun_lasser);
	wid_image_destroy(image_gun_mine);
	wid_image_destroy(image_gun_bombball);

	wid_image_destroy(image_bonus_speed);
	wid_image_destroy(image_bonus_shot);
	wid_image_destroy(image_bonus_teleport);
	wid_image_destroy(image_bonus_ghost);
	wid_image_destroy(image_bonus_4x);
	wid_image_destroy(image_bonus_hidden);

#ifndef NO_SOUND
	check_destroy(check_music);
	check_destroy(check_sound);
#endif

	check_destroy(check_ai);

	for (i = GUN_DUAL_SIMPLE; i <= GUN_BOMBBALL; i++) {
		check_destroy(check[i]);
	}

	for (i = BONUS_SPEED; i <= BONUS_HIDDEN; i++) {
		check_destroy(check[i]);
	}

	button_destroy(button_back);
	button_destroy(button_keys);
}
