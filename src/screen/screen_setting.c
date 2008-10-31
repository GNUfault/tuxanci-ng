
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

#ifndef NO_SOUND
#    include "music.h"
#    include "sound.h"
#endif

#include "screen_mainMenu.h"
#include "screen_setting.h"
#include "screen_settingKeys.h"
#include "screen_settingKeys.h"
#include "screen_choiceArena.h"

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

void startScreenSetting()
{
#ifndef NO_SOUND
	playMusic("menu", MUSIC_GROUP_BASE);
#endif
}

void drawScreenSetting()
{
	int i;

	drawWidgetImage(image_backgorund);

	drawWidgetLabel(label_count_round);
	drawWidgetLabel(label_name_player1);
	drawWidgetLabel(label_ai);

#ifndef NO_SOUND
	drawWidgetLabel(label_music);
	drawWidgetLabel(label_sound);
#endif

	drawWidgetTextfield(textfield_count_cound);
	drawWidgetTextfield(textfield_name_player1);

	if (getWidgetCheckStatus(check_ai) == FALSE) {
		drawWidgetLabel(label_name_player2);
		drawWidgetTextfield(textfield_name_player2);
	}

	drawWidgetImage(image_gun_dual_revolver);
	drawWidgetImage(image_gun_scatter);
	drawWidgetImage(image_gun_tommy);
	drawWidgetImage(image_gun_lasser);
	drawWidgetImage(image_gun_mine);
	drawWidgetImage(image_gun_bombball);

	drawWidgetImage(image_bonus_speed);
	drawWidgetImage(image_bonus_shot);
	drawWidgetImage(image_bonus_teleport);
	drawWidgetImage(image_bonus_ghost);
	drawWidgetImage(image_bonus_4x);
	drawWidgetImage(image_bonus_hidden);

#ifndef NO_SOUND
	drawWidgetCheck(check_music);
	drawWidgetCheck(check_sound);
#endif

	for (i = GUN_DUAL_SIMPLE; i <= GUN_BOMBBALL; i++) {
		drawWidgetCheck(check[i]);
	}

	for (i = BONUS_SPEED; i <= BONUS_HIDDEN; i++) {
		drawWidgetCheck(check[i]);
	}

	drawWidgetCheck(check_ai);

	drawWidgetButton(button_back);
	drawWidgetButton(button_keys);
}

void eventScreenSetting()
{
	int i;

	eventWidgetTextfield(textfield_count_cound);
	eventWidgetTextfield(textfield_name_player1);

	if (getWidgetCheckStatus(check_ai) == FALSE) {
		eventWidgetTextfield(textfield_name_player2);
	}
#ifndef NO_SOUND
	eventWidgetCheck(check_music);
	eventWidgetCheck(check_sound);
#endif

	for (i = GUN_DUAL_SIMPLE; i <= GUN_BOMBBALL; i++) {
		eventWidgetCheck(check[i]);
	}

	for (i = BONUS_SPEED; i <= BONUS_HIDDEN; i++) {
		eventWidgetCheck(check[i]);
	}

	eventWidgetCheck(check_ai);

	eventWidgetButton(button_back);
	eventWidgetButton(button_keys);
}

void stopScreenSetting()
{
}

static void eventWidget(void *p)
{
	widget_t *button;
	widget_t *check;

	button = (widget_t *) (p);
	check = (widget_t *) (p);

	if (button == button_back) {
		setScreen("mainMenu");
	}
	if (button == button_keys) {
		setScreen("settingKeys");
	}
#ifndef NO_SOUND
	if (check == check_music) {
		setMusicActive(getWidgetCheckStatus(check_music));
	}

	if (check == check_sound) {
		setSoundActive(getWidgetCheckStatus(check_sound));
	}
#endif
}

static void initSettingFile()
{
	char path[STR_PATH_SIZE];
	char val[STR_SIZE];

	sprintf(path, "%s/tuxanci.conf", getHomeDirector());

	configFile = loadTextFile(path);

	if (configFile == NULL) {
		fprintf(stderr, _("I am unable to load: \"%s\"!\n"), path);
		fprintf(stderr, _("Creating: \"%s\"\n"), path);

		configFile = newTextFile(path);
	}

	if (configFile == NULL) {
		fprintf(stderr, _("I was unable to create: \"%s\"!\n"), path);
		return;
	}

	loadValueFromConfigFile(configFile, "COUNT_ROUND", val, STR_SIZE, "15");
	setWidgetTextFiledText(textfield_count_cound, val);

	loadValueFromConfigFile(configFile, "NAME_PLAYER_RIGHT", val, STR_SIZE,
							NAME_PLAYER_RIGHT);
	setWidgetTextFiledText(textfield_name_player1, val);

	loadValueFromConfigFile(configFile, "NAME_PLAYER_LEFT", val, STR_SIZE,
							NAME_PLAYER_LEFT);
	setWidgetTextFiledText(textfield_name_player2, val);

	loadValueFromConfigFile(configFile, "GUN_DUAL_SIMPLE", val, STR_SIZE,
							"YES");
	setWidgetCheckStatus(check[GUN_DUAL_SIMPLE], isYesOrNO(val));
	loadValueFromConfigFile(configFile, "GUN_SCATTER", val, STR_SIZE, "YES");
	setWidgetCheckStatus(check[GUN_SCATTER], isYesOrNO(val));
	loadValueFromConfigFile(configFile, "GUN_TOMMY", val, STR_SIZE, "YES");
	setWidgetCheckStatus(check[GUN_TOMMY], isYesOrNO(val));
	loadValueFromConfigFile(configFile, "GUN_LASSER", val, STR_SIZE, "YES");
	setWidgetCheckStatus(check[GUN_LASSER], isYesOrNO(val));
	loadValueFromConfigFile(configFile, "GUN_MINE", val, STR_SIZE, "YES");
	setWidgetCheckStatus(check[GUN_MINE], isYesOrNO(val));
	loadValueFromConfigFile(configFile, "GUN_BOMBBALL", val, STR_SIZE, "YES");
	setWidgetCheckStatus(check[GUN_BOMBBALL], isYesOrNO(val));

	loadValueFromConfigFile(configFile, "BONUS_SPEED", val, STR_SIZE, "YES");
	setWidgetCheckStatus(check[BONUS_SPEED], isYesOrNO(val));
	loadValueFromConfigFile(configFile, "BONUS_SHOT", val, STR_SIZE, "YES");
	setWidgetCheckStatus(check[BONUS_SHOT], isYesOrNO(val));
	loadValueFromConfigFile(configFile, "BONUS_TELEPORT", val, STR_SIZE,
							"YES");
	setWidgetCheckStatus(check[BONUS_TELEPORT], isYesOrNO(val));
	loadValueFromConfigFile(configFile, "BONUS_GHOST", val, STR_SIZE, "YES");
	setWidgetCheckStatus(check[BONUS_GHOST], isYesOrNO(val));
	loadValueFromConfigFile(configFile, "BONUS_4X", val, STR_SIZE, "YES");
	setWidgetCheckStatus(check[BONUS_4X], isYesOrNO(val));
	loadValueFromConfigFile(configFile, "BONUS_HIDDEN", val, STR_SIZE, "YES");
	setWidgetCheckStatus(check[BONUS_HIDDEN], isYesOrNO(val));

	loadValueFromConfigFile(configFile, "AI", val, STR_SIZE, "NO");
	setWidgetCheckStatus(check_ai, isYesOrNO(val));

#ifndef NO_SOUND
	loadValueFromConfigFile(configFile, "MUSIC", val, STR_SIZE, "YES");
	setWidgetCheckStatus(check_music, isYesOrNO(val));

	loadValueFromConfigFile(configFile, "SOUND", val, STR_SIZE, "YES");
	setWidgetCheckStatus(check_sound, isYesOrNO(val));
#endif

	loadValueFromConfigFile(configFile, "ARENA", val, STR_SIZE, "FAGN");
	setChoiceArena(getArenaFileFormNetName(val));

	saveTextFile(configFile);
}

static void saveAndDestroyConfigFile()
{
	if (configFile == NULL) {
		fprintf(stderr,
				_
				("I am unable to save configure file, because config file was not initialised!\n"));

		return;
	}

	setValueInConfigFile(configFile, "COUNT_ROUND",
						 getTextFromWidgetTextfield(textfield_count_cound));
	setValueInConfigFile(configFile, "NAME_PLAYER_RIGHT",
						 getTextFromWidgetTextfield(textfield_name_player1));
	setValueInConfigFile(configFile, "NAME_PLAYER_LEFT",
						 getTextFromWidgetTextfield(textfield_name_player2));

	setValueInConfigFile(configFile, "GUN_DUAL_SIMPLE",
						 getYesOrNo(getWidgetCheckStatus
									(check[GUN_DUAL_SIMPLE])));
	setValueInConfigFile(configFile, "GUN_SCATTER",
						 getYesOrNo(getWidgetCheckStatus(check[GUN_SCATTER])));
	setValueInConfigFile(configFile, "GUN_TOMMY",
						 getYesOrNo(getWidgetCheckStatus(check[GUN_TOMMY])));
	setValueInConfigFile(configFile, "GUN_LASSER",
						 getYesOrNo(getWidgetCheckStatus(check[GUN_LASSER])));
	setValueInConfigFile(configFile, "GUN_MINE",
						 getYesOrNo(getWidgetCheckStatus(check[GUN_MINE])));
	setValueInConfigFile(configFile, "GUN_BOMBBALL",
						 getYesOrNo(getWidgetCheckStatus
									(check[GUN_BOMBBALL])));

	setValueInConfigFile(configFile, "BONUS_SPEED",
						 getYesOrNo(getWidgetCheckStatus(check[BONUS_SPEED])));
	setValueInConfigFile(configFile, "BONUS_SHOT",
						 getYesOrNo(getWidgetCheckStatus(check[BONUS_SHOT])));
	setValueInConfigFile(configFile, "BONUS_TELEPORT",
						 getYesOrNo(getWidgetCheckStatus
									(check[BONUS_TELEPORT])));
	setValueInConfigFile(configFile, "BONUS_GHOST",
						 getYesOrNo(getWidgetCheckStatus(check[BONUS_GHOST])));
	setValueInConfigFile(configFile, "BONUS_4X",
						 getYesOrNo(getWidgetCheckStatus(check[BONUS_4X])));
	setValueInConfigFile(configFile, "BONUS_HIDDEN",
						 getYesOrNo(getWidgetCheckStatus
									(check[BONUS_HIDDEN])));

	setValueInConfigFile(configFile, "AI",
						 getYesOrNo(getWidgetCheckStatus(check_ai)));

#ifndef NO_SOUND
	setValueInConfigFile(configFile, "MUSIC",
						 getYesOrNo(getWidgetCheckStatus(check_music)));
	setValueInConfigFile(configFile, "SOUND",
						 getYesOrNo(getWidgetCheckStatus(check_sound)));
#endif

	if( getChoiceArena() != NULL )
	{
		setValueInConfigFile(configFile, "ARENA",
							getArenaNetName(getChoiceArena()));
	}
	//TODO

	saveTextFile(configFile);
	destroyTextFile(configFile);
}

void initScreenSetting()
{
	image_t *image;
	int i;

	image = getImage(IMAGE_GROUP_BASE, "screen_main");
	image_backgorund = newWidgetImage(0, 0, image);

	button_back =
		newWidgetButton(_("back"), WINDOW_SIZE_X - 200, WINDOW_SIZE_Y - 100,
						eventWidget);
	button_keys =
		newWidgetButton(_("controls"), WINDOW_SIZE_X - 200,
						button_back->y - WIDGET_BUTTON_HEIGHT - 10,
						eventWidget);

#ifndef NO_SOUND
	label_music =
		newWidgetLabel(_("Music:"), 100, WINDOW_SIZE_Y - 85,
					   WIDGET_LABEL_LEFT);
	check_music =
		newWidgetCheck(label_music->x + label_music->w + 10,
					   WINDOW_SIZE_Y - 80, isMusicActive(), eventWidget);
	label_sound =
		newWidgetLabel(_("Sound:"), check_music->x + WIDGET_CHECK_WIDTH + 10,
					   WINDOW_SIZE_Y - 85, WIDGET_LABEL_LEFT);
	check_sound =
		newWidgetCheck(label_sound->x + label_sound->w + 10,
					   WINDOW_SIZE_Y - 80, isSoundActive(), eventWidget);
	label_ai =
		newWidgetLabel(_("AI:"), check_sound->x + WIDGET_CHECK_WIDTH + 10,
					   WINDOW_SIZE_Y - 85, WIDGET_LABEL_LEFT);
#else
	label_ai =
		newWidgetLabel(_("AI:"), 100, WINDOW_SIZE_Y - 85, WIDGET_LABEL_LEFT);
#endif

	check_ai =
		newWidgetCheck(label_ai->x + label_ai->w + 10, WINDOW_SIZE_Y - 80,
					   FALSE, eventWidget);

	label_count_round =
		newWidgetLabel(_("No. of rounds:"), 100, WINDOW_SIZE_Y - 200,
					   WIDGET_LABEL_LEFT);
	label_name_player1 =
		newWidgetLabel(_("Player 1:"), 100, WINDOW_SIZE_Y - 160,
					   WIDGET_LABEL_LEFT);
	label_name_player2 =
		newWidgetLabel(_("Player 2:"), 100, WINDOW_SIZE_Y - 120,
					   WIDGET_LABEL_LEFT);

	textfield_count_cound = newWidgetTextfield(getParamElse("--count", "15"),
											   WIDGET_TEXTFIELD_FILTER_NUM,
											   110 + label_count_round->w,
											   WINDOW_SIZE_Y - 200);

	textfield_name_player1 =
		newWidgetTextfield(getParamElse("--name1", NAME_PLAYER_RIGHT),
						   WIDGET_TEXTFIELD_FILTER_ALPHANUM,
						   110 + label_count_round->w, WINDOW_SIZE_Y - 160);

	textfield_name_player2 =
		newWidgetTextfield(getParamElse("--name2", NAME_PLAYER_LEFT),
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

		check[i] = newWidgetCheck(x, y, TRUE, NULL);
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

		check[i] = newWidgetCheck(x, y, TRUE, NULL);
	}

	image_gun_dual_revolver =
		newWidgetImage(110 + WIDGET_CHECK_WIDTH, 200,
					   getImage(IMAGE_GROUP_BASE, "panel_dual"));
	image_gun_scatter =
		newWidgetImage(110 + WIDGET_CHECK_WIDTH, 250,
					   getImage(IMAGE_GROUP_BASE, "panel_scatter"));
	image_gun_tommy =
		newWidgetImage(110 + WIDGET_CHECK_WIDTH, 300,
					   getImage(IMAGE_GROUP_BASE, "panel_tommy"));
	image_gun_lasser =
		newWidgetImage(260 + WIDGET_CHECK_WIDTH, 200,
					   getImage(IMAGE_GROUP_BASE, "panel_lasser"));
	image_gun_mine =
		newWidgetImage(260 + WIDGET_CHECK_WIDTH, 250,
					   getImage(IMAGE_GROUP_BASE, "panel_mine"));
	image_gun_bombball =
		newWidgetImage(260 + WIDGET_CHECK_WIDTH, 300,
					   getImage(IMAGE_GROUP_BASE, "panel_bombball"));

	image_bonus_speed =
		newWidgetImage(430 + WIDGET_CHECK_WIDTH, 200,
					   getImage(IMAGE_GROUP_BASE, "panel_speed"));;
	image_bonus_shot =
		newWidgetImage(430 + WIDGET_CHECK_WIDTH, 250,
					   getImage(IMAGE_GROUP_BASE, "panel_shot"));;
	image_bonus_teleport =
		newWidgetImage(430 + WIDGET_CHECK_WIDTH, 300,
					   getImage(IMAGE_GROUP_BASE, "panel_teleport"));;
	image_bonus_ghost =
		newWidgetImage(580 + WIDGET_CHECK_WIDTH, 200,
					   getImage(IMAGE_GROUP_BASE, "panel_ghost"));;
	image_bonus_4x =
		newWidgetImage(580 + WIDGET_CHECK_WIDTH, 250,
					   getImage(IMAGE_GROUP_BASE, "panel_4x"));;
	image_bonus_hidden =
		newWidgetImage(580 + WIDGET_CHECK_WIDTH, 300,
					   getImage(IMAGE_GROUP_BASE, "panel_hidden"));;

	registerScreen(newScreen
				   ("setting", startScreenSetting, eventScreenSetting,
					drawScreenSetting, stopScreenSetting));

	initSettingFile();

#ifndef NO_SOUND
	eventWidget(check_music);
	eventWidget(check_sound);
#endif
}

void getSettingNameRight(char *s)
{
	strcpy(s, getTextFromWidgetTextfield(textfield_name_player1));
}

void getSettingNameLeft(char *s)
{
	if (getWidgetCheckStatus(check_ai)) {
		strcpy(s, NAME_AI);
	} else {
		strcpy(s, getTextFromWidgetTextfield(textfield_name_player2));
	}
}

void getSettingCountRound(int *n)
{
	*n = atoi(getTextFromWidgetTextfield(textfield_count_cound));
}

bool_t isSettingAI()
{
	return getWidgetCheckStatus(check_ai);
}

bool_t isSettingAnyItem()
{
	int i;

	for (i = GUN_DUAL_SIMPLE; i <= GUN_BOMBBALL; i++) {
		if (getWidgetCheckStatus(check[i]) == TRUE) {
			return TRUE;
		}
	}

	for (i = BONUS_SPEED; i < BONUS_HIDDEN; i++) {
		if (getWidgetCheckStatus(check[i]) == TRUE) {
			return TRUE;
		}
	}

	return FALSE;
}

bool_t isSettingItem(int item)
{
	return getWidgetCheckStatus(check[item]);
}

void quitScreenSetting()
{
	int i;

	saveAndDestroyConfigFile();

	destroyWidgetImage(image_backgorund);

	destroyWidgetLabel(label_count_round);
	destroyWidgetLabel(label_name_player1);
	destroyWidgetLabel(label_ai);

#ifndef NO_SOUND
	destroyWidgetLabel(label_music);
	destroyWidgetLabel(label_sound);
#endif
	destroyWidgetLabel(label_name_player2);
	destroyWidgetTextfield(textfield_name_player2);

	destroyWidgetTextfield(textfield_name_player1);
	destroyWidgetTextfield(textfield_count_cound);

	destroyWidgetImage(image_gun_dual_revolver);
	destroyWidgetImage(image_gun_scatter);
	destroyWidgetImage(image_gun_tommy);
	destroyWidgetImage(image_gun_lasser);
	destroyWidgetImage(image_gun_mine);
	destroyWidgetImage(image_gun_bombball);

	destroyWidgetImage(image_bonus_speed);
	destroyWidgetImage(image_bonus_shot);
	destroyWidgetImage(image_bonus_teleport);
	destroyWidgetImage(image_bonus_ghost);
	destroyWidgetImage(image_bonus_4x);
	destroyWidgetImage(image_bonus_hidden);

#ifndef NO_SOUND
	destroyWidgetCheck(check_music);
	destroyWidgetCheck(check_sound);
#endif

	destroyWidgetCheck(check_ai);

	for (i = GUN_DUAL_SIMPLE; i <= GUN_BOMBBALL; i++) {
		destroyWidgetCheck(check[i]);
	}

	for (i = BONUS_SPEED; i <= BONUS_HIDDEN; i++) {
		destroyWidgetCheck(check[i]);
	}

	destroyWidgetButton(button_back);
	destroyWidgetButton(button_keys);
}
