
#include <stdio.h>
#include <assert.h>

#include "main.h"
#include "interface.h"
#include "screen.h"
#include "image.h"
#include "music.h"
#include "sound.h"
#include "screen_mainMenu.h"
#include "list.h"
#include "screen_setting.h"
#include "tux.h"

#include "widget_label.h"
#include "widget_button.h"
#include "widget_image.h"
#include "widget_textfield.h"
#include "widget_check.h"

static widget_image_t *image_backgorund;

static widget_button_t *button_back;

static widget_label_t *label_count_round;
static widget_label_t *label_name_player1;
static widget_label_t *label_name_player2;
static widget_label_t *label_music;
static widget_label_t *label_sound;

static widget_check_t *check[ITEM_COUNT];
static widget_check_t *check_music;
static widget_check_t *check_sound;

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

void startScreenSetting()
{
	playMusic("menu", MUSIC_GROUP_BASE);
}

void drawScreenSetting()
{
	int i;

	drawWidgetImage(image_backgorund);
	
	drawWidgetLabel(label_count_round);
	drawWidgetLabel(label_name_player1);
	drawWidgetLabel(label_name_player2);
	drawWidgetLabel(label_music);
	drawWidgetLabel(label_sound);

	drawWidgetTextfield(textfield_count_cound);
	drawWidgetTextfield(textfield_name_player1);
	drawWidgetTextfield(textfield_name_player2);

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

	drawWidgetCheck(check_music);
	drawWidgetCheck(check_sound);

	for( i = GUN_DUAL_SIMPLE ; i <= GUN_BOMBBALL ; i++ )
	{
		drawWidgetCheck(check[i]);
	}

	for( i = BONUS_SPEED ; i <= BONUS_HIDDEN ; i++ )
	{
		drawWidgetCheck(check[i]);
	}

	drawWidgetButton(button_back);
}

void eventScreenSetting()
{
	int i;

	eventWidgetTextfield(textfield_count_cound);
	eventWidgetTextfield(textfield_name_player1);
	eventWidgetTextfield(textfield_name_player2);

	eventWidgetCheck(check_music);
	eventWidgetCheck(check_sound);

	for( i = GUN_DUAL_SIMPLE ; i <= GUN_BOMBBALL ; i++ )
	{
		eventWidgetCheck(check[i]);
	}

	for( i = BONUS_SPEED ; i <= BONUS_HIDDEN ; i++ )
	{
		eventWidgetCheck(check[i]);
	}

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

	if( check == check_music )
	{
		setMusicActive( check_music->status );
	}

	if( check == check_sound )
	{
		setSoundActive( check_sound->status );
	}
}

void initScreenSetting()
{
	SDL_Surface *image;
	int i;

	image = getImage(IMAGE_GROUP_BASE, "screen_main");
	image_backgorund  = newWidgetImage(0, 0, image);

	button_back = newWidgetButton("Back", WINDOW_SIZE_X-200, WINDOW_SIZE_Y-100, eventWidget);

	label_count_round = newWidgetLabel("Count Round :", 100, WINDOW_SIZE_Y-200, WIDGET_LABEL_LEFT);
	label_name_player1 = newWidgetLabel("Name player1 :", 100, WINDOW_SIZE_Y-160, WIDGET_LABEL_LEFT);
	label_name_player2 = newWidgetLabel("Name player2 :", 100, WINDOW_SIZE_Y-120, WIDGET_LABEL_LEFT);

	textfield_count_cound = newWidgetTextfield(getParamElse("--count", "15"), 110+label_count_round->w, WINDOW_SIZE_Y-200);
	
	textfield_name_player1 = newWidgetTextfield(getParamElse("--name1", "name1"), 110+label_name_player1->w, WINDOW_SIZE_Y-160);
	textfield_name_player2 = newWidgetTextfield(getParamElse("--name2", "name2"), 110+label_name_player2->w, WINDOW_SIZE_Y-120);

	label_music = newWidgetLabel("Music :", 100, WINDOW_SIZE_Y-85, WIDGET_LABEL_LEFT);
	check_music = newWidgetCheck(label_music->x + label_music->w  + 10,
		WINDOW_SIZE_Y-80, isMusicActive() , eventWidget);
	label_sound = newWidgetLabel("Sound :", check_music->x + WIDGET_CHECK_WIDTH + 10,
		WINDOW_SIZE_Y-85, WIDGET_LABEL_LEFT);
	check_sound = newWidgetCheck(label_sound->x + label_sound->w + 10,
		WINDOW_SIZE_Y-80, isSoundActive() , eventWidget);

	for( i = GUN_DUAL_SIMPLE ; i <= GUN_BOMBBALL ; i++ )
	{
		int x;
		int y;

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
		int x;
		int y;

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
	image_gun_bombball = newWidgetImage(260 + WIDGET_CHECK_WIDTH, 300, getImage(IMAGE_GROUP_BASE, "panel_dual"));

	image_bonus_speed = newWidgetImage(430 + WIDGET_CHECK_WIDTH, 200, getImage(IMAGE_GROUP_BASE, "panel_speed"));;
	image_bonus_shot = newWidgetImage(430 + WIDGET_CHECK_WIDTH, 250, getImage(IMAGE_GROUP_BASE, "panel_shot"));;
	image_bonus_teleport = newWidgetImage(430 + WIDGET_CHECK_WIDTH, 300, getImage(IMAGE_GROUP_BASE, "panel_teleport"));;
	image_bonus_ghost = newWidgetImage(580 + WIDGET_CHECK_WIDTH, 200, getImage(IMAGE_GROUP_BASE, "panel_ghost"));;
	image_bonus_4x = newWidgetImage(580 + WIDGET_CHECK_WIDTH, 250, getImage(IMAGE_GROUP_BASE, "panel_4x"));;
	image_bonus_hidden = newWidgetImage(580 + WIDGET_CHECK_WIDTH, 300, getImage(IMAGE_GROUP_BASE, "panel_hidden"));;

	registerScreen( newScreen("setting", startScreenSetting, eventScreenSetting,
		drawScreenSetting, stopScreenSetting) );
}

void getSettingNameRight(char *s)
{
	strcpy(s, textfield_name_player1->text);
}

void getSettingNameLeft(char *s)
{
	strcpy(s, textfield_name_player2->text);
}

void getSettingCountRound(int *n)
{
	*n = atoi( textfield_count_cound->text );
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

	for( i = BONUS_SPEED ; i <= BONUS_HIDDEN ; i++ )
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

	destroyWidgetImage(image_backgorund);

	destroyWidgetLabel(label_count_round);
	destroyWidgetLabel(label_name_player1);
	destroyWidgetLabel(label_name_player2);
	destroyWidgetLabel(label_music);
	destroyWidgetLabel(label_sound);

	destroyWidgetTextfield(textfield_count_cound);
	destroyWidgetTextfield(textfield_name_player1);
	destroyWidgetTextfield(textfield_name_player2);

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

	destroyWidgetCheck(check_music);
	destroyWidgetCheck(check_sound);

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

