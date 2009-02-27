
#include <stdio.h>
#include <assert.h>

#include "main.h"
#include "list.h"
#include "arenaFile.h"

#include "interface.h"
#include "screen.h"
#include "image.h"
#include "hotKey.h"

#ifndef NO_SOUND
#    include "music.h"
#endif

#include "screen_mainMenu.h"
#include "screen_choiceArena.h"

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

//static list_t *listWidgetButtonimage; // map buttons to be showed and handled - well, show_map_buttons is used instead
static list_t *listAllWidgetButtonimage; // all map buttons
static int handled_map_button_group; // 0 - first 6 buttons, 1 - second 6 buttons, ....
static void activateHandledMapButtonGroup();
static struct show_map_buttons { 
	// geez, it'd be nicer to used two lists
	// but how the hell is list_t supposed to be used?! :-)
	int min; // see activateHandledMapButtonGroup for desc
	int max; //
	int next; // 0: max is last item+1 in listAllWidgetButtonimage, otherwise 1
	int prev; // 0: min is first item in listAllWidgetButtonimage, otherwise 1
} show_map_buttons = { 0,0,0,0 } ;
static arenaFile_t *currentArena;

static void hotkey_escape()
{
	setScreen("gameType");
}

void startScreenChoiceArena()
{
#ifndef NO_SOUND
	playMusic("menu", MUSIC_GROUP_BASE);
#endif

	registerHotKey(SDLK_ESCAPE, hotkey_escape);
}

void drawScreenChoiceArena()
{
	int i;

	drawWidgetImage(image_backgorund);

	drawWidgetButton(button_play);
	drawWidgetButton(button_back);
	if (show_map_buttons.next) { drawWidgetButton(button_next); }
	if (show_map_buttons.prev) { drawWidgetButton(button_prev); }

	for (i = show_map_buttons.min ; i < show_map_buttons.max; i++) {
		drawWidgetButtonimage((widget_t*)listAllWidgetButtonimage->list[i]);
	}
}

void eventScreenChoiceArena()
{
	int i;

	eventWidgetButton(button_play);
	eventWidgetButton(button_back);
	if (show_map_buttons.next) { eventWidgetButton(button_next); }
	if (show_map_buttons.prev) { eventWidgetButton(button_prev); }

	for (i = show_map_buttons.min ; i < show_map_buttons.max; i++) {
		eventWidgetButtonimage((widget_t*)listAllWidgetButtonimage->list[i]);
	}
}

void stopScreenChoiceArena()
{
	unregisterHotKey(SDLK_ESCAPE);
}

static void eventWidgetButtonImage(void *p)
{
	widget_t *buttonimage;
	int i;

	buttonimage = (widget_t *) (p);

	for (i = show_map_buttons.min; i < show_map_buttons.max; i++) { 
		widget_t *this;

		this = (widget_t *) (listAllWidgetButtonimage->list[i]);

		if (buttonimage == this) {
			setWidgetButtonimageActive(this, TRUE);
			currentArena = getArenaFile(i);
		} else {
			setWidgetButtonimageActive(this, FALSE);
		}
	}
}

arenaFile_t *getChoiceArena()
{
	return currentArena;
}

void setChoiceArena(arenaFile_t * arenaFile)
{
	widget_t *widget_buttonimage;
	int id;

	id = getArenaFileID(arenaFile);
	currentArena = arenaFile;

	if (id >= 0) {
		widget_buttonimage = (widget_t *) listAllWidgetButtonimage->list[id];
		setWidgetButtonimageActive(widget_buttonimage, TRUE);
	}
}

static void eventWidget(void *p)
{
	widget_t *button;

	button = (widget_t *) (p);

	if (button == button_play) {
		setScreen("world");
	}

	if (button == button_back) {
		setScreen("gameType");
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

// groups of 6 map buttons to be shown and handled
// the group is set by static global variable handled_map_button_group
// sets static global show_map_buttons.min to index of first button to be drawn in listAllButtonimage
// and show_map_buttons.max to index of last button to be drawn+1 in listAllButtonimage
static void activateHandledMapButtonGroup(void) {
	int min = handled_map_button_group * 6;
	min = min < 0 ? 0 : min;
	min = min <= getArenaCount() ? min : getArenaCount();
	int max = min+6 <= getArenaCount() ? min+6 : getArenaCount();

	if (min) { show_map_buttons.prev = 1; }
	else { show_map_buttons.prev = 0; }
	if (max < getArenaCount()) { show_map_buttons.next = 1; }
	else { show_map_buttons.next = 0; }
	show_map_buttons.min = min;
	show_map_buttons.max = max;
}

void initScreenChoiceArena()
{
	image_t *image;
	int i;

	image = getImage(IMAGE_GROUP_BASE, "screen_main");
	image_backgorund = newWidgetImage(0, 0, image);

	button_back = newWidgetButton(_("back"), 100, WINDOW_SIZE_Y - 100, eventWidget);
	button_play = newWidgetButton(_("play"), WINDOW_SIZE_X - 200, WINDOW_SIZE_Y - 100, eventWidget);

	button_prev = newWidgetButton(_("prev maps"), 250, WINDOW_SIZE_Y - 100, eventWidget);
	button_next = newWidgetButton(_("next maps"), WINDOW_SIZE_X-350, WINDOW_SIZE_Y - 100, eventWidget);

	listAllWidgetButtonimage = newList();
	currentArena = NULL;

	for (i = 0; i < getArenaCount(); i++) {
		widget_t *widget_buttonimage;
		arenaFile_t *arenaFile;
		int x, y;

		arenaFile = getArenaFile(i);
		//image = addImageData(getArenaImage(i), IMAGE_NO_ALPHA, "none", IMAGE_GROUP_BASE);
		image = loadImageFromArena(arenaFile,  getArenaImage(arenaFile),
					   IMAGE_GROUP_BASE, "none", IMAGE_NO_ALPHA);

		// only 6 map buttons can be shown at once;
		// set buttons' positions in each group to the corresponding values
		// handled_map_button_group and activateHandledMapButtonGroup is used 
		// to choose group to be shown and handled
		int pos = i % 6 ;
		x = 100 + 200 * (pos - 3 * (pos / 3));
		y = 150 + 200 * (pos / 3);

		widget_buttonimage = newWidgetButtonimage(image, x, y, eventWidgetButtonImage);
/*
		if( i == choiceArenaId )
		{
			widget_buttonimage->active = 1;
		}
*/
		addList(listAllWidgetButtonimage, widget_buttonimage);
		handled_map_button_group = 0;
		activateHandledMapButtonGroup();
	}

	registerScreen( newScreen("chiceArena", startScreenChoiceArena,
			eventScreenChoiceArena, drawScreenChoiceArena,
			stopScreenChoiceArena));
}

void quitScreenChoiceArena()
{
	destroyWidgetImage(image_backgorund);

	destroyWidgetButton(button_play);
	destroyWidgetButton(button_back);

	destroyWidgetButton(button_next);
	destroyWidgetButton(button_prev);

	destroyListItem(listAllWidgetButtonimage, destroyWidgetButtonimage);
}
