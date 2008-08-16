
#include <stdio.h>
#include <assert.h>

#include "main.h"

#include "game.h"
#include "interface.h"
#include "language.h"
#include "screen.h"
#include "image.h"

#ifndef NO_SOUND
#include "music.h"
#endif

#include "screen_mainMenu.h"
#include "screen_setting.h"

#include "widget.h"
#include "widget_label.h"
#include "widget_button.h"
#include "widget_textfield.h"
#include "widget_check.h"
#include "widget_image.h"

static widget_t *image_backgorund;

static widget_t *button_play;
static widget_t *button_setting;
static widget_t *button_table;
static widget_t *button_credits;
static widget_t *button_end;

void startScreenMainMenu()
{
#ifndef NO_SOUND
	playMusic("menu", MUSIC_GROUP_BASE);
#endif
}

void drawScreenMainMenu()
{
	drawWidgetImage(image_backgorund);

	drawWidgetButton(button_play);
	drawWidgetButton(button_setting);
	drawWidgetButton(button_table);
	drawWidgetButton(button_credits);
	drawWidgetButton(button_end);
}

void eventScreenMainMenu()
{
	eventWidgetButton(button_play);
	eventWidgetButton(button_setting);
	eventWidgetButton(button_table);
	eventWidgetButton(button_credits);
	eventWidgetButton(button_end);
}

void stopScreenMainMenu()
{
}

static void eventWidget(void *p)
{
	widget_t *button;
	
	button = (widget_t *)(p);

	if( button == button_play )
	{
		setScreen("gameType");
	}

	if( button == button_setting )
	{
		setScreen("setting");
	}

	if( button == button_table )
	{
		setScreen("table");
	}

	if( button == button_credits )
	{
		setScreen("credits");
	}

	if( button == button_end )
	{
		quitGame();
	}
}

void initScreenMainMenu()
{
	image_t *image;

	image = addImageData("screen_main.png", IMAGE_NO_ALPHA, "screen_main", IMAGE_GROUP_BASE);
	image_backgorund  = newWidgetImage(0, 0, image);

	button_play = newWidgetButton(getMyText("PLAY"), WINDOW_SIZE_X/2 -WIDGET_BUTTON_WIDTH/2, 200, eventWidget);
	button_setting = newWidgetButton(getMyText("SETTING"), WINDOW_SIZE_X/2 -WIDGET_BUTTON_WIDTH/2, 250, eventWidget);
	button_table = newWidgetButton(getMyText("TABLE"), WINDOW_SIZE_X/2 -WIDGET_BUTTON_WIDTH/2, 300, eventWidget);
	button_credits = newWidgetButton(getMyText("CREDITS"), WINDOW_SIZE_X/2 -WIDGET_BUTTON_WIDTH/2, 350, eventWidget);
	button_end = newWidgetButton(getMyText("END"), WINDOW_SIZE_X/2 -WIDGET_BUTTON_WIDTH/2, 400, eventWidget);

#ifndef NO_SOUND
	addMusic("menu.ogg", "menu", MUSIC_GROUP_BASE);
#endif

	registerScreen( newScreen("mainMenu", startScreenMainMenu, eventScreenMainMenu,
		drawScreenMainMenu, stopScreenMainMenu) );
}

void quitScreenMainMenu()
{
	destroyWidgetImage(image_backgorund);

	destroyWidgetButton(button_play);
	destroyWidgetButton(button_setting);
	destroyWidgetButton(button_table);
	destroyWidgetButton(button_credits);
	destroyWidgetButton(button_end);
}

