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
#include "config.h"

#include "configFile.h"
#include "interface.h"
#include "screen.h"
#include "image.h"
#include "hotKey.h"

#ifndef NO_SOUND
#include "music.h"
#include "sound.h"
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
#include "widget_statusbar.h"

static widget_t *image_backgorund;

static widget_t *button_back;
static widget_t *button_keys;		/* setting of keycontrols */

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

static widget_t *statusbar;

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

	statusbar_draw(statusbar);

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

	statusbar_event(statusbar);

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

	my_button = (widget_t *) p;
	my_check = (widget_t *) p;

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
	text_field_set_text(textfield_count_cound, get_string_static(config_get_int_value(CFG_COUNT_ROUND)));
	text_field_set_text(textfield_name_player1, config_get_str_value(CFG_NAME_RIGHT));
	text_field_set_text(textfield_name_player2, config_get_str_value(CFG_NAME_LEFT));

	check_set_status(check[GUN_DUAL_SIMPLE], config_get_int_value(CFG_GUN_DUAL_SIMPLE));
	check_set_status(check[GUN_SCATTER], config_get_int_value(CFG_GUN_SCATTER));
	check_set_status(check[GUN_TOMMY], config_get_int_value(CFG_GUN_TOMMY));
	check_set_status(check[GUN_LASSER], config_get_int_value(CFG_GUN_LASSER));
	check_set_status(check[GUN_MINE], config_get_int_value(CFG_GUN_MINE));
	check_set_status(check[GUN_BOMBBALL], config_get_int_value(CFG_GUN_BOMBBALL));

	check_set_status(check[BONUS_SPEED], config_get_int_value(CFG_BONUS_SPEED));
	check_set_status(check[BONUS_SHOT], config_get_int_value(CFG_BONUS_SHOT));
	check_set_status(check[BONUS_TELEPORT], config_get_int_value(CFG_BONUS_TELEPORT));
	check_set_status(check[BONUS_GHOST], config_get_int_value(CFG_BONUS_GHOST));
	check_set_status(check[BONUS_4X], config_get_int_value(CFG_BONUS_4X));
	check_set_status(check[BONUS_HIDDEN], config_get_int_value(CFG_BONUS_HIDDEN));

	check_set_status(check_ai, config_get_int_value(CFG_AI));

#ifndef NO_SOUND
	check_set_status(check_music, config_get_int_value(CFG_MUSIC));
	check_set_status(check_sound, config_get_int_value(CFG_SOUND));
#endif
}

static void saveAndDestroyConfigFile()
{
	config_set_int_value(CFG_COUNT_ROUND, atoi(text_field_get_text(textfield_count_cound)));
	config_set_str_value(CFG_NAME_RIGHT, text_field_get_text(textfield_name_player1));
	config_set_str_value(CFG_NAME_LEFT, text_field_get_text(textfield_name_player2));

	config_set_int_value(CFG_GUN_DUAL_SIMPLE, check_get_status(check[GUN_DUAL_SIMPLE]));
	config_set_int_value(CFG_GUN_SCATTER, check_get_status(check[GUN_SCATTER]));
	config_set_int_value(CFG_GUN_TOMMY, check_get_status(check[GUN_TOMMY]));
	config_set_int_value(CFG_GUN_LASSER, check_get_status(check[GUN_LASSER]));
	config_set_int_value(CFG_GUN_MINE, check_get_status(check[GUN_MINE]));
	config_set_int_value(CFG_GUN_BOMBBALL, check_get_status(check[GUN_BOMBBALL]));
	config_set_int_value(CFG_BONUS_SPEED, check_get_status(check[BONUS_SPEED]));
	config_set_int_value(CFG_BONUS_SHOT, check_get_status(check[BONUS_SHOT]));
	config_set_int_value(CFG_BONUS_TELEPORT, check_get_status(check[BONUS_TELEPORT]));
	config_set_int_value(CFG_BONUS_GHOST, check_get_status(check[BONUS_GHOST]));
	config_set_int_value(CFG_BONUS_4X, check_get_status(check[BONUS_4X]));
	config_set_int_value(CFG_BONUS_HIDDEN, check_get_status(check[BONUS_HIDDEN]));

	config_set_int_value(CFG_AI, check_get_status(check_ai));

#ifndef NO_SOUND
	config_set_int_value(CFG_MUSIC, check_get_status(check_music));
	config_set_int_value(CFG_SOUND, check_get_status(check_sound));
#endif
}

void setting_init()
{
	image_t *image;
	int i;

	image = image_get(IMAGE_GROUP_BASE, "screen_main");
	image_backgorund = wid_image_new(0, 0, image);

	button_back = button_new(_("Back"), WINDOW_SIZE_X - 200, WINDOW_SIZE_Y - 100, eventWidget);

	button_keys = button_new(_("Controls"), WINDOW_SIZE_X - 200,
				 button_back->y - WIDGET_BUTTON_HEIGHT - 10, eventWidget);

#ifndef NO_SOUND
	label_music = label_new(_("Music:"), 100, WINDOW_SIZE_Y - 85, WIDGET_LABEL_LEFT);

	check_music = check_new(label_music->x + label_music->w + 10, WINDOW_SIZE_Y - 80,
				music_is_active(), eventWidget);
	label_sound = label_new(_("Sound:"), check_music->x + WIDGET_CHECK_WIDTH + 10,
				WINDOW_SIZE_Y - 85, WIDGET_LABEL_LEFT);

	check_sound = check_new(label_sound->x + label_sound->w + 10, WINDOW_SIZE_Y - 80,
				sound_is_active(), eventWidget);

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

	statusbar = statusbar_new(400, 350);
	statusbar_add(statusbar, check[GUN_DUAL_SIMPLE], _("Dual simple gun"));
	statusbar_add(statusbar, check[GUN_SCATTER], _("Scatter"));
	statusbar_add(statusbar, check[GUN_TOMMY], _("Tommy"));
	statusbar_add(statusbar, check[GUN_LASSER], _("Laser beam"));
	statusbar_add(statusbar, check[GUN_MINE], _("Mine"));
	statusbar_add(statusbar, check[GUN_BOMBBALL], _("Bomb ball"));

	statusbar_add(statusbar, check[BONUS_SPEED], _("Speed bonus"));
	statusbar_add(statusbar, check[BONUS_SHOT], _("Infinite ammo"));
	statusbar_add(statusbar, check[BONUS_TELEPORT], _("Unkillable bonus"));
	statusbar_add(statusbar, check[BONUS_GHOST], _("No-walls bonus"));
	statusbar_add(statusbar, check[BONUS_4X], _("1 shot to 4 directions bonus"));
	statusbar_add(statusbar, check[BONUS_HIDDEN], _("Unvisibility bonus"));

	screen_register(screen_new("setting", screen_startSetting, setting_event,
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

	statusbar_destroy(statusbar);

	button_destroy(button_back);
	button_destroy(button_keys);
}
