
#include <stdio.h>
#include <assert.h>

#include "main.h"

#include "game.h"
#include "interface.h"
#include "screen.h"
#include "image.h"
#include "hotKey.h"

#ifndef NO_SOUND
#    include "music.h"
#endif

#include "mainMenu.h"
#include "setting.h"

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

static void hotkey_escape()
{
	game_quit();
}

void main_menu_start()
{
#ifndef NO_SOUND
	music_play("menu", MUSIC_GROUP_BASE);
#endif

	hot_key_register(SDLK_ESCAPE, hotkey_escape);
}

void main_menu_draw()
{
	wid_image_draw(image_backgorund);

	button_draw(button_play);
	button_draw(button_setting);
	button_draw(button_table);
	button_draw(button_credits);
	button_draw(button_end);
}

void main_menu_event()
{
	button_event(button_play);
	button_event(button_setting);
	button_event(button_table);
	button_event(button_credits);
	button_event(button_end);
}

void main_menu_stop()
{
	hot_key_unregister(SDLK_ESCAPE);
}

static void eventWidget(void *p)
{
	widget_t *button;

	button = (widget_t *) (p);

	if (button == button_play) {
		screen_set("gameType");
	}

	if (button == button_setting) {
		screen_set("setting");
	}

	if (button == button_table) {
		screen_set("table");
	}

	if (button == button_credits) {
		screen_set("credits");
	}

	if (button == button_end) {
		game_quit();
	}
}

void main_menu_init()
{
	image_t *image;

	image =
		image_add("screen_main.png", IMAGE_NO_ALPHA, "screen_main",
					 IMAGE_GROUP_BASE);
	image_backgorund = wid_image_new(0, 0, image);

	button_play = button_new(_("Start game"), WINDOW_SIZE_X / 2 - WIDGET_BUTTON_WIDTH / 2, 200, eventWidget);
	button_setting = button_new(_("Settings"), WINDOW_SIZE_X / 2 - WIDGET_BUTTON_WIDTH / 2, 250, eventWidget);
	button_table = button_new(_("Highscore"), WINDOW_SIZE_X / 2 - WIDGET_BUTTON_WIDTH / 2, 300, eventWidget);
	button_credits = button_new(_("Credits"), WINDOW_SIZE_X / 2 - WIDGET_BUTTON_WIDTH / 2, 350, eventWidget);
	button_end = button_new(_("Quit game"), WINDOW_SIZE_X / 2 - WIDGET_BUTTON_WIDTH / 2, 400, eventWidget);

#ifndef NO_SOUND
	music_add("menu.ogg", "menu", MUSIC_GROUP_BASE);
#endif

	screen_register( screen_new("mainMenu", main_menu_start, main_menu_event,
			main_menu_draw, main_menu_stop));
}

void main_menu_quit()
{
	wid_image_destroy(image_backgorund);

	button_destroy(button_play);
	button_destroy(button_setting);
	button_destroy(button_table);
	button_destroy(button_credits);
	button_destroy(button_end);
}
