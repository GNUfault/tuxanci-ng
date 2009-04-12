
#include <stdio.h>
#include <assert.h>

#include "main.h"
#include "list.h"
#include "textFile.h"

#include "interface.h"
#include "screen.h"
#include "image.h"
#include "hotKey.h"

#ifndef NO_SOUND
#    include "music.h"
#endif

#include "credits.h"

#include "widget.h"
#include "widget_image.h"
#include "widget_label.h"
#include "widget_button.h"

static widget_t *image_backgorund;
static widget_t *button_back;
static list_t *listWidgetLabel;
static textFile_t *textFile;
static int offset;
static int creditExists;

static void hotkey_escape()
{
	screen_set("mainMenu");
}

void scredits_start()
{
#ifndef NO_SOUND
	music_play("menu", MUSIC_GROUP_BASE);
#endif
	offset = 0;

	hotKey_register(SDLK_ESCAPE, hotkey_escape);
}

void scredits_draw()
{
	int i;

	wid_image_draw(image_backgorund);

	button_draw(button_back);

	for (i = 0; i < listWidgetLabel->count; i++) {
		int z;

		widget_t *this;

		this = (widget_t *) listWidgetLabel->list[i];

		z = this->y;
		this->y += offset;

		if (this->y > SCREEN_CREDITS_OFFSET_MIN && this->y < SCREEN_CREDITS_OFFSET_MAX) {
			label_draw(this);
		}

		this->y = z;
	}

}

void scredits_event()
{
	button_event(button_back);

	offset -= SCREEN_CREDITS_OFFSET_SPEED;

	if (offset < SCREEN_CREDITS_OFFSET_RESTART) {
		offset = 0;
	}

	//printf("offset = %d\n", offset);
}

void scredits_stop()
{
	unhotKey_register(SDLK_ESCAPE);
}

static void eventWidget(void *p)
{
	widget_t *button;

	button = (widget_t *) (p);

	if (button == button_back) {
		screen_set("mainMenu");
	}
}

void scredits_init()
{
	image_t *image;
	int i;

	image = image_get(IMAGE_GROUP_BASE, "screen_main");
	image_backgorund = wid_image_new(0, 0, image);

	button_back = button_new(_("back"), WINDOW_SIZE_X / 2 - WIDGET_BUTTON_WIDTH / 2,
						 WINDOW_SIZE_Y - 80, eventWidget);

	listWidgetLabel = list_new();

	if (tryExistFile(PATH_DOC SCREEN_CREDITS_FILE) == 0) {
		creditExists = 1;
		textFile = textFile_load(PATH_DOC SCREEN_CREDITS_FILE);

		for (i = 0; i < textFile->text->count; i++) {
			widget_t *label;
			char *line;

			line = (char *) textFile->text->list[i];

			label = label_new(line, WINDOW_SIZE_X / 2 - WINDOW_SIZE_X / 4,
						(WINDOW_SIZE_Y - 100) + i * 20,
						 WIDGET_LABEL_LEFT);

			list_add(listWidgetLabel, label);
		}
	} else {
		for (i = 0; i < 5; i++) {
			creditExists = 0;
			widget_t *label;
			char line[STR_SIZE];

			sprintf(line, _("Credit file not found... %s/%s"), PATH_DOC,
					SCREEN_CREDITS_FILE);

			label = label_new(line, WINDOW_SIZE_X / 2,
						(WINDOW_SIZE_Y - 100) + i * 20,
						WIDGET_LABEL_CENTER);

			list_add(listWidgetLabel, label);
		}
	}

	screen_register(screen_new ("credits", scredits_start, scredits_event,
				scredits_draw, scredits_stop));
}

void scredits_quit()
{
	wid_image_destroy(image_backgorund);

	button_destroy(button_back);
	list_destroy_item(listWidgetLabel, label_destroy);

	if (creditExists) {
		textFile_destroy(textFile);
	}
}
