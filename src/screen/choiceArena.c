#include <stdio.h>
#include <assert.h>

#include "main.h"
#include "list.h"
#include "arenaFile.h"

#include "interface.h"
#include "screen.h"
#include "image.h"
#include "hotKey.h"
#include "config.h"

#ifndef NO_SOUND
#include "music.h"
#endif

#include "mainMenu.h"
#include "choiceArena.h"

#include "widget.h"
#include "widget_label.h"
#include "widget_button.h"
#include "widget_image.h"
#include "widget_buttonimage.h"

static widget_t *image_backgorund;

static widget_t *button_play;
static widget_t *button_back;
static widget_t *button_next;
static widget_t *button_prev;

/*static list_t *listWidgetButtonimage;*/	/* map buttons to be showed and handled - well, show_map_buttons is used instead now */
static list_t *listAllWidgetButtonimage;	/* all map buttons */
static int handled_map_button_group;		/* 0 - first 6 buttons, 1 - second 6 buttons, ... */
static void activateHandledMapButtonGroup();
static struct show_map_buttons { 
	/* geez, it'd be nicer to use two lists
	   but how the hell list_t is supposed to be used?! :-) */
	int min;	/* see activateHandledMapButtonGroup for description */
	int max;
	int next;	/* 0: max is last item+1 in listAllWidgetButtonimage, otherwise 1 */
	int prev;	/* 0: min is first item in listAllWidgetButtonimage, otherwise 1 */
} show_map_buttons = { 0,0,0,0 };
static arenaFile_t *currentArena;

static void hotkey_escape()
{
	screen_set("gameType");
}

void screen_startChoiceArena()
{
#ifndef NO_SOUND
	music_play("menu", MUSIC_GROUP_BASE);
#endif

	hot_key_register(SDLK_ESCAPE, hotkey_escape);
}

void choice_arena_draw()
{
	int i;

	wid_image_draw(image_backgorund);

	button_draw(button_play);
	button_draw(button_back);
	if (show_map_buttons.next) { button_draw(button_next); }
	if (show_map_buttons.prev) { button_draw(button_prev); }

	for (i = show_map_buttons.min; i < show_map_buttons.max; i++) {
		button_image_draw((widget_t *) listAllWidgetButtonimage->list[i]);
	}
}

void choice_arena_event()
{
	int i;

	button_event(button_play);
	button_event(button_back);
	if (show_map_buttons.next) { button_event(button_next); }
	if (show_map_buttons.prev) { button_event(button_prev); }

	for (i = show_map_buttons.min ; i < show_map_buttons.max; i++) {
		button_image_event((widget_t *) listAllWidgetButtonimage->list[i]);
	}
}

void stopScreenChoiceArena()
{
	hot_key_unregister(SDLK_ESCAPE);
}

static void button_eventImage(void *p)
{
	widget_t *buttonimage;
	int i;

	buttonimage = (widget_t *) p;

	for (i = show_map_buttons.min; i < show_map_buttons.max; i++) { 
		widget_t *this;

		this = (widget_t *) listAllWidgetButtonimage->list[i];

		if (buttonimage == this) {
			button_image_set_active(this, TRUE);
			currentArena = arena_file_get(i);
		} else {
			button_image_set_active(this, FALSE);
		}
	}
}

arenaFile_t *choice_arena_get()
{
	return currentArena;
}

void choice_arena_set(arenaFile_t *arenaFile)
{
	widget_t *widget_buttonimage;
	int id;

	id = arena_file_get_id(arenaFile);
	currentArena = arenaFile;

	if (id >= 0) {
		widget_buttonimage = (widget_t *) listAllWidgetButtonimage->list[id];
		button_image_set_active(widget_buttonimage, TRUE);
	}
}

static void eventWidget(void *p)
{
	widget_t *button;

	button = (widget_t *) p;

	if (button == button_play) {
		screen_set("world");
	}

	if (button == button_back) {
		screen_set("gameType");
	}

	if (button == button_next) {
		if (show_map_buttons.next) {
			handled_map_button_group++;
			activateHandledMapButtonGroup();
		}
	}

	if (button == button_prev) {
		if (show_map_buttons.prev) {
			handled_map_button_group--;
			activateHandledMapButtonGroup();
		}
	}
}

/**
 * groups of 6 map buttons to be shown and handled
 * the group is set by static global variable handled_map_button_group
 * sets static global show_map_buttons.min to index of first button to be drawn in listAllButtonimage
 * and show_map_buttons.max to index of last button to be drawn+1 in listAllButtonimage
 */
static void activateHandledMapButtonGroup(void)
{
	int min = handled_map_button_group * 6;
	min = min < 0 ? 0 : min;
	min = min <= arena_file_get_count() ? min : arena_file_get_count();
	int max = min + 6 <= arena_file_get_count() ? min + 6 : arena_file_get_count();

	if (min) {
		show_map_buttons.prev = 1;
	} else {
		show_map_buttons.prev = 0;
	}

	if (max < arena_file_get_count()) {
		show_map_buttons.next = 1;
	} else {
		show_map_buttons.next = 0;
	}

	show_map_buttons.min = min;
	show_map_buttons.max = max;
}

void choice_arena_init()
{
	image_t *image;
	int i;

	image = image_get(IMAGE_GROUP_BASE, "screen_main");
	image_backgorund = wid_image_new(0, 0, image);

	button_back = button_new(_("Back"), 100, WINDOW_SIZE_Y - 100, eventWidget);
	button_play = button_new(_("Play"), WINDOW_SIZE_X - 200, WINDOW_SIZE_Y - 100, eventWidget);

	button_prev = button_new(_("Previous arenas"), 250, WINDOW_SIZE_Y - 100, eventWidget);
	button_next = button_new(_("Next arenas"), WINDOW_SIZE_X - 350, WINDOW_SIZE_Y - 100, eventWidget);

	listAllWidgetButtonimage = list_new();
	currentArena = NULL;

	for (i = 0; i < arena_file_get_count(); i++) {
		widget_t *widget_buttonimage;
		arenaFile_t *arenaFile;
		int x, y;

		arenaFile = arena_file_get(i);
		/*image = image_add(arena_file_get_image(i), IMAGE_NO_ALPHA, "none", IMAGE_GROUP_BASE);*/
		image = arena_file_load_image(arenaFile,  arena_file_get_image(arenaFile),
					      IMAGE_GROUP_BASE, "none", IMAGE_NO_ALPHA);

		/*
		  only 6 map buttons can be shown at once;
		  set buttons' positions in each group to the corresponding values
		  handled_map_button_group and activateHandledMapButtonGroup is used 
		  to choose group to be shown and handled
		*/
		int pos = i % 6 ;
		x = 100 + 200 * (pos - 3 * (pos / 3));
		y = 150 + 200 * (pos / 3);

		widget_buttonimage = button_image_new(image, x, y, button_eventImage);
/*
		if (i == choiceArenaId) {
			widget_buttonimage->active = 1;
		}
*/
		list_add(listAllWidgetButtonimage, widget_buttonimage);
		handled_map_button_group = 0;
		activateHandledMapButtonGroup();
	}

	screen_register(screen_new("chiceArena", screen_startChoiceArena,
			choice_arena_event, choice_arena_draw,
			stopScreenChoiceArena));

	choice_arena_set(arena_file_get_file_format_net_name(config_get_str_value(CFG_ARENA)));
}

void choice_arena_quit()
{
	if (choice_arena_get() != NULL) {
		config_set_str_value(CFG_ARENA, arena_file_get_net_name(choice_arena_get()));
	}

	wid_image_destroy(image_backgorund);

	button_destroy(button_play);
	button_destroy(button_back);

	button_destroy(button_next);
	button_destroy(button_prev);

	list_destroy_item(listAllWidgetButtonimage, button_image_destroy);
}
