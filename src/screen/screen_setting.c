
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "main.h"
#include "list.h"
#include "tux.h"
#include "textFile.h"
#include "director.h"
#include "homeDirector.h"

#include "configFile.h"
#include "language.h"
#include "interface.h"
#include "screen.h"
#include "image.h"

#ifndef NO_SOUND
#include "music.h"
#include "sound.h"
#endif

#include "screen_mainMenu.h"
#include "screen_setting.h"

#include "widget_label.h"
#include "widget_button.h"
#include "widget_image.h"
#include "widget_textfield.h"
#include "widget_check.h"
#include "widget_select.h"

static widget_image_t *image_backgorund;

static widget_button_t *button_back;

static widget_label_t *label_count_round;
static widget_label_t *label_name_player1;
static widget_label_t *label_name_player2;
static widget_label_t *label_ai;

#ifndef NO_SOUND
static widget_label_t *label_music;
static widget_label_t *label_sound;
#endif

static widget_check_t *check[ITEM_COUNT];
#ifndef NO_SOUND
static widget_check_t *check_music;
static widget_check_t *check_sound;
#endif

static widget_image_t *image_gun_dual_revolver;
static widget_image_t *image_gun_scatter;
static widget_image_t *image_gun_tommy;
static widget_image_t *image_gun_lasser;
static widget_image_t *image_gun_mine;
static widget_image_t *image_gun_bombball;

static widget_image_t *image_bonus_speed;
static widget_image_t *image_bonus_shot;
static widget_image_t *image_bonus_teleport;
static widget_image_t *image_bonus_ghost;
static widget_image_t *image_bonus_4x;
static widget_image_t *image_bonus_hidden;

static widget_textfield_t *textfield_count_cound;
static widget_textfield_t *textfield_name_player1;
static widget_textfield_t *textfield_name_player2;

static widget_check_t *check_ai;

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

	if ( check_ai->status == 0 )
	{
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

	for( i = GUN_DUAL_SIMPLE ; i <= GUN_BOMBBALL ; i++ )
	{
		drawWidgetCheck(check[i]);
	}

	for( i = BONUS_SPEED ; i <= BONUS_HIDDEN ; i++ )
	{
		drawWidgetCheck(check[i]);
	}

	drawWidgetCheck(check_ai);

	drawWidgetButton(button_back);
}

void eventScreenSetting()
{
	int i;

	eventWidgetTextfield(textfield_count_cound);
	eventWidgetTextfield(textfield_name_player1);

	if ( check_ai->status == 0 )
	{
		eventWidgetTextfield(textfield_name_player2);
	}

#ifndef NO_SOUND
	eventWidgetCheck(check_music);
	eventWidgetCheck(check_sound);
#endif

	for( i = GUN_DUAL_SIMPLE ; i <= GUN_BOMBBALL ; i++ )
	{
		eventWidgetCheck(check[i]);
	}

	for( i = BONUS_SPEED ; i <= BONUS_HIDDEN ; i++ )
	{
		eventWidgetCheck(check[i]);
	}

	eventWidgetCheck(check_ai);

	eventWidgetButton(button_back);
}

void stopScreenSetting()
{
}

static void eventWidget(void *p)
{
	widget_button_t *button;
	widget_check_t *check;
	
	button = (widget_button_t *)(p);
	check = (widget_check_t *)(p);

	if( button == button_back )
	{
		setScreen("mainMenu");
	}

#ifndef NO_SOUND
	if( check == check_music )
	{
		setMusicActive( check_music->status );
	}

	if( check == check_sound )
	{
		setSoundActive( check_sound->status );
	}
#endif
}

static void initSettingFile()
{
	char path[STR_PATH_SIZE];
	char val[STR_SIZE];

	sprintf(path, "%s/tuxanci-ng.conf", getHomeDirector());

	configFile = loadTextFile(path);

	if( configFile == NULL )
	{
		fprintf(stderr, "Don't load %s\n", path);
		fprintf(stderr, "I create %s\n", path);

		configFile = newTextFile(path);
	}

	if( configFile == NULL )
	{
		fprintf(stderr, "Don't create %s\n", path);
		return;
	}

	loadValueFromConfigFile(configFile, "COUNT_ROUND", val, STR_SIZE, "15");
	setWidgetTextFiledText(textfield_count_cound, val);

	loadValueFromConfigFile(configFile, "NAME_PLAYER_RIGHT", val, STR_SIZE, NAME_PLAYER_RIGHT);
	setWidgetTextFiledText(textfield_name_player1, val);

	loadValueFromConfigFile(configFile, "NAME_PLAYER_LEFT", val, STR_SIZE, NAME_PLAYER_LEFT);
	setWidgetTextFiledText(textfield_name_player2, val);

	loadValueFromConfigFile(configFile, "GUN_DUAL_SIMPLE", val, STR_SIZE, "YES");
	check[GUN_DUAL_SIMPLE]->status = isYesOrNO(val);
	loadValueFromConfigFile(configFile, "GUN_SCATTER", val, STR_SIZE, "YES");
	check[GUN_SCATTER]->status = isYesOrNO(val);
	loadValueFromConfigFile(configFile, "GUN_TOMMY", val, STR_SIZE, "YES");
	check[GUN_TOMMY]->status = isYesOrNO(val);
	loadValueFromConfigFile(configFile, "GUN_LASSER", val, STR_SIZE, "YES");
	check[GUN_LASSER]->status = isYesOrNO(val);
	loadValueFromConfigFile(configFile, "GUN_MINE", val, STR_SIZE, "YES");
	check[GUN_MINE]->status = isYesOrNO(val);
	loadValueFromConfigFile(configFile, "GUN_BOMBBALL", val, STR_SIZE, "YES");
	check[GUN_BOMBBALL]->status = isYesOrNO(val);

	loadValueFromConfigFile(configFile, "BONUS_SPEED", val, STR_SIZE, "YES");
	check[BONUS_SPEED]->status = isYesOrNO(val);
	loadValueFromConfigFile(configFile, "BONUS_SHOT", val, STR_SIZE, "YES");
	check[BONUS_SHOT]->status = isYesOrNO(val);
	loadValueFromConfigFile(configFile, "BONUS_TELEPORT", val, STR_SIZE, "YES");
	check[BONUS_TELEPORT]->status = isYesOrNO(val);
	loadValueFromConfigFile(configFile, "BONUS_GHOST", val, STR_SIZE, "YES");
	check[BONUS_GHOST]->status = isYesOrNO(val);
	loadValueFromConfigFile(configFile, "BONUS_4X", val, STR_SIZE, "YES");
	check[BONUS_4X]->status = isYesOrNO(val);
	loadValueFromConfigFile(configFile, "BONUS_HIDDEN", val, STR_SIZE, "YES");
	check[BONUS_HIDDEN]->status = isYesOrNO(val);

	loadValueFromConfigFile(configFile, "AI", val, STR_SIZE, "NO");
	check_ai->status = isYesOrNO(val);

#ifndef NO_SOUND
	loadValueFromConfigFile(configFile, "MUSIC", val, STR_SIZE, "YES");
	check_music->status = isYesOrNO(val);

	loadValueFromConfigFile(configFile, "SOUND", val, STR_SIZE, "YES");
	check_sound->status = isYesOrNO(val);
#endif

	saveTextFile(configFile);
}

static void saveAndDestroyConfigFile()
{
	if( configFile == NULL )
	{
		fprintf(stderr, "i can't save configure, "
			"because config file not created !\n");

		return;
	}

	setValueInConfigFile(configFile, "COUNT_ROUND", textfield_count_cound->text );
	setValueInConfigFile(configFile, "NAME_PLAYER_RIGHT", textfield_name_player1->text );
	setValueInConfigFile(configFile, "NAME_PLAYER_LEFT", textfield_name_player2->text );

	setValueInConfigFile(configFile, "GUN_DUAL_SIMPLE", getYesOrNo(check[GUN_DUAL_SIMPLE]->status) );
	setValueInConfigFile(configFile, "GUN_SCATTER", getYesOrNo(check[GUN_SCATTER]->status) );
	setValueInConfigFile(configFile, "GUN_TOMMY", getYesOrNo(check[GUN_TOMMY]->status) );
	setValueInConfigFile(configFile, "GUN_LASSER", getYesOrNo(check[GUN_LASSER]->status) );
	setValueInConfigFile(configFile, "GUN_MINE", getYesOrNo(check[GUN_MINE]->status) );
	setValueInConfigFile(configFile, "GUN_BOMBBALL", getYesOrNo(check[GUN_BOMBBALL]->status) );

	setValueInConfigFile(configFile, "BONUS_SPEED", getYesOrNo(check[BONUS_SPEED]->status) );
	setValueInConfigFile(configFile, "BONUS_SHOT", getYesOrNo(check[BONUS_SHOT]->status) );
	setValueInConfigFile(configFile, "BONUS_TELEPORT", getYesOrNo(check[BONUS_TELEPORT]->status) );
	setValueInConfigFile(configFile, "BONUS_GHOST", getYesOrNo(check[BONUS_GHOST]->status) );
	setValueInConfigFile(configFile, "BONUS_4X", getYesOrNo(check[BONUS_4X]->status) );
	setValueInConfigFile(configFile, "BONUS_HIDDEN", getYesOrNo(check[BONUS_HIDDEN]->status) );

	setValueInConfigFile(configFile, "AI", getYesOrNo(check_ai->status) );

#ifndef NO_SOUND
	setValueInConfigFile(configFile, "MUSIC", getYesOrNo(check_music->status) );
	setValueInConfigFile(configFile, "SOUND", getYesOrNo(check_sound->status) );
#endif

	saveTextFile(configFile);
	destroyTextFile(configFile);
}

void initScreenSetting()
{
	image_t *image;
	int i;

	image = getImage(IMAGE_GROUP_BASE, "screen_main");
	image_backgorund  = newWidgetImage(0, 0, image);

	button_back = newWidgetButton(getMyText("BACK"), WINDOW_SIZE_X-200, WINDOW_SIZE_Y-100, eventWidget);

#ifndef NO_SOUND
	label_music = newWidgetLabel(getMyText("MUSIC"), 100, WINDOW_SIZE_Y-85, WIDGET_LABEL_LEFT);
	check_music = newWidgetCheck(label_music->x + label_music->w  + 10,
		WINDOW_SIZE_Y-80, isMusicActive() , eventWidget);
	label_sound = newWidgetLabel(getMyText("SOUND"), check_music->x + WIDGET_CHECK_WIDTH + 10,
		WINDOW_SIZE_Y-85, WIDGET_LABEL_LEFT);
	check_sound = newWidgetCheck(label_sound->x + label_sound->w + 10,
		WINDOW_SIZE_Y-80, isSoundActive() , eventWidget);
	label_ai = newWidgetLabel("AI:", check_sound->x + WIDGET_CHECK_WIDTH + 10, WINDOW_SIZE_Y-85, WIDGET_LABEL_LEFT);
#else
	label_ai = newWidgetLabel("AI:", 100, WINDOW_SIZE_Y-85, WIDGET_LABEL_LEFT);
#endif

	check_ai = newWidgetCheck(label_ai->x + label_ai->w + 10, WINDOW_SIZE_Y-80, FALSE, eventWidget);

	label_count_round = newWidgetLabel(getMyText("COUNT_ROUND"), 100, WINDOW_SIZE_Y-200, WIDGET_LABEL_LEFT);
	label_name_player1 = newWidgetLabel(getMyText("NAME_PLAYER1"), 100, WINDOW_SIZE_Y-160, WIDGET_LABEL_LEFT);
	label_name_player2 = newWidgetLabel(getMyText("NAME_PLAYER2"), 100, WINDOW_SIZE_Y-120, WIDGET_LABEL_LEFT);

	textfield_count_cound = newWidgetTextfield(
		getParamElse("--count", "15"),
		WIDGET_TEXTFIELD_FILTER_NUM,
		110+label_count_round->w,
		WINDOW_SIZE_Y-200);
	
	textfield_name_player1 = newWidgetTextfield(
		getParamElse("--name1", NAME_PLAYER_RIGHT),
		WIDGET_TEXTFIELD_FILTER_ALPHANUM,
		110+label_name_player1->w,
		WINDOW_SIZE_Y-160);

	textfield_name_player2 = newWidgetTextfield(
		getParamElse("--name2", NAME_PLAYER_LEFT),
		WIDGET_TEXTFIELD_FILTER_ALPHANUM,
		110+label_name_player2->w,
		WINDOW_SIZE_Y-120);

	for( i = GUN_DUAL_SIMPLE ; i <= GUN_BOMBBALL ; i++ )
	{
		int x = 0;
		int y = 0;

		if( i >= GUN_DUAL_SIMPLE && i <= GUN_TOMMY )
		{
			x = 100;
			y = 200 + ( i - GUN_DUAL_SIMPLE ) * 50;
		}

		if( i >= GUN_LASSER && i <= GUN_BOMBBALL )
		{
			x = 250;
			y = 200 + ( i - GUN_LASSER ) * 50;
		}

		check[i] = newWidgetCheck(x, y, TRUE, NULL);
	}

	for( i = BONUS_SPEED ; i <= BONUS_HIDDEN ; i++ )
	{
		int x = 0;
		int y = 0;

		if( i >= BONUS_SPEED && i <= BONUS_TELEPORT )
		{
			x = 430;
			y = 200 + ( i - BONUS_SPEED ) * 50;
		}

		if( i >= BONUS_GHOST && i <= BONUS_HIDDEN )
		{
			x = 580;
			y = 200 + ( i - BONUS_GHOST ) * 50;
		}

		check[i] = newWidgetCheck(x, y, TRUE, NULL);
	}

	image_gun_dual_revolver = newWidgetImage(110 + WIDGET_CHECK_WIDTH, 200, getImage(IMAGE_GROUP_BASE, "panel_dual"));
	image_gun_scatter = newWidgetImage(110 + WIDGET_CHECK_WIDTH, 250, getImage(IMAGE_GROUP_BASE, "panel_scatter"));
	image_gun_tommy = newWidgetImage(110 + WIDGET_CHECK_WIDTH, 300, getImage(IMAGE_GROUP_BASE, "panel_tommy"));
	image_gun_lasser = newWidgetImage(260 + WIDGET_CHECK_WIDTH, 200, getImage(IMAGE_GROUP_BASE, "panel_lasser"));
	image_gun_mine = newWidgetImage(260 + WIDGET_CHECK_WIDTH, 250, getImage(IMAGE_GROUP_BASE, "panel_mine"));
	image_gun_bombball = newWidgetImage(260 + WIDGET_CHECK_WIDTH, 300, getImage(IMAGE_GROUP_BASE, "panel_bombball"));

	image_bonus_speed = newWidgetImage(430 + WIDGET_CHECK_WIDTH, 200, getImage(IMAGE_GROUP_BASE, "panel_speed"));;
	image_bonus_shot = newWidgetImage(430 + WIDGET_CHECK_WIDTH, 250, getImage(IMAGE_GROUP_BASE, "panel_shot"));;
	image_bonus_teleport = newWidgetImage(430 + WIDGET_CHECK_WIDTH, 300, getImage(IMAGE_GROUP_BASE, "panel_teleport"));;
	image_bonus_ghost = newWidgetImage(580 + WIDGET_CHECK_WIDTH, 200, getImage(IMAGE_GROUP_BASE, "panel_ghost"));;
	image_bonus_4x = newWidgetImage(580 + WIDGET_CHECK_WIDTH, 250, getImage(IMAGE_GROUP_BASE, "panel_4x"));;
	image_bonus_hidden = newWidgetImage(580 + WIDGET_CHECK_WIDTH, 300, getImage(IMAGE_GROUP_BASE, "panel_hidden"));;

	registerScreen( newScreen("setting", startScreenSetting, eventScreenSetting,
		drawScreenSetting, stopScreenSetting) );

	initSettingFile();

#ifndef NO_SOUND
	eventWidget(check_music);
	eventWidget(check_sound);
#endif
}

void getSettingNameRight(char *s)
{
	strcpy(s, textfield_name_player1->text);
}

void getSettingNameLeft(char *s)
{
	if( check_ai->status )
	{
		strcpy(s, NAME_AI);
	}
	else
	{
		strcpy(s, textfield_name_player2->text);
	}
}

void getSettingCountRound(int *n)
{
	*n = atoi( textfield_count_cound->text );
}

bool_t isSettingAI()
{
	return check_ai->status;
}

bool_t isSettingAnyItem()
{
	int i;

	for( i = GUN_DUAL_SIMPLE ; i <= GUN_BOMBBALL ; i++ )
	{
		if( check[i]->status == TRUE )
		{
			return TRUE;
		}
	}

	for( i = BONUS_SPEED ; i < BONUS_HIDDEN ; i++ )
	{
		if( check[i]->status == TRUE )
		{
			return TRUE;
		}
	}

	return FALSE;
}

bool_t isSettingItem(int item)
{
	return check[item]->status;
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

	for( i = GUN_DUAL_SIMPLE ; i <= GUN_BOMBBALL ; i++ )
	{
		destroyWidgetCheck(check[i]);
	}

	for( i = BONUS_SPEED ; i <= BONUS_HIDDEN ; i++ )
	{
		destroyWidgetCheck(check[i]);
	}

	destroyWidgetButton(button_back);
}

