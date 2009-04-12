
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "main.h"
#include "list.h"
#include "textFile.h"
#include "homeDirector.h"

#include "interface.h"
#include "screen.h"
#include "image.h"
#include "hotKey.h"

#ifndef NO_SOUND
#    include "music.h"
#endif

#include "table.h"

#include "widget_image.h"
#include "widget_label.h"
#include "widget_button.h"

static widget_t *image_backgorund;
static widget_t *button_back;

static list_t *listWidgetLabelNumer;
static list_t *listWidgetLabelName;
static list_t *listWidgetLabelScore;

static textFile_t *textFile;

static void hotkey_escape()
{
	screen_set("mainMenu");
}

void table_start()
{
#ifndef NO_SOUND
	music_play("menu", MUSIC_GROUP_BASE);
#endif

	hot_key_register(SDLK_ESCAPE, hotkey_escape);
}

void table_draw()
{
	int i;

	wid_image_draw(image_backgorund);

	button_draw(button_back);

	for (i = 0; i < listWidgetLabelNumer->count; i++) {
		widget_t *this;
		this = (widget_t *) listWidgetLabelNumer->list[i];
		label_draw(this);
	}

	for (i = 0; i < listWidgetLabelName->count; i++) {
		widget_t *this;
		this = (widget_t *) listWidgetLabelName->list[i];
		label_draw(this);
	}

	for (i = 0; i < listWidgetLabelScore->count; i++) {
		widget_t *this;
		this = (widget_t *) listWidgetLabelScore->list[i];
		label_draw(this);
	}
}

void table_event()
{
	button_event(button_back);
}

void table_stop()
{
	hot_key_unregister(SDLK_ESCAPE);
}

static void eventWidget(void *p)
{
	widget_t *button;

	button = (widget_t *) (p);

	if (button == button_back) {
		screen_set("mainMenu");
	}
}

static void setWidgetLabel()
{
	int i;

	if (textFile == NULL) {
		fprintf(stderr, _("Highscore file: \"%s\" was not loaded!"), SCREEN_TABLE_FILE_HIGHSCORE_NAME);

		return;
	}

	if (listWidgetLabelNumer != NULL ||
	    listWidgetLabelName != NULL ||
	    listWidgetLabelScore != NULL) {
		list_destroy_item(listWidgetLabelNumer, label_destroy);
		list_destroy_item(listWidgetLabelName, label_destroy);
		list_destroy_item(listWidgetLabelScore, label_destroy);
	}

	listWidgetLabelNumer = list_new();
	listWidgetLabelName = list_new();
	listWidgetLabelScore = list_new();

	for (i = 0; i < SCREEN_TABLE_MAX_PLAYERS; i++) {
		widget_t *label;
		char name[STR_NAME_SIZE];
		char score[STR_NUM_SIZE];
		char num[STR_NUM_SIZE];
		char *line;

		line = (char *) textFile->text->list[i];

		sscanf(line, "%s %s", name, score);
		sprintf(num, "%2d)", i + 1);

		label = label_new(num, WINDOW_SIZE_X / 2 - 100, 200 + i * 20, WIDGET_LABEL_LEFT);
		list_add(listWidgetLabelNumer, label);

		label = label_new(name, WINDOW_SIZE_X / 2, 200 + i * 20, WIDGET_LABEL_CENTER);
		list_add(listWidgetLabelName, label);

		label = label_new(score, WINDOW_SIZE_X / 2 + 80, 200 + i * 20, WIDGET_LABEL_LEFT);
		list_add(listWidgetLabelScore, label);
	}
}

static void loadHighscoreFile()
{
	char path[STR_PATH_SIZE];
	int i;

	sprintf(path, "%s/" SCREEN_TABLE_FILE_HIGHSCORE_NAME, home_director_get());
	textFile = text_file_load(path);

	if (textFile == NULL) {
		fprintf(stderr, _("I am unable to load: \"%s\"!\n"), path);
		fprintf(stderr, _("Creating: \"%s\"\n"), path);
		textFile = text_file_new(path);
	} else {
		return;
	}

	if (textFile == NULL) {
		fprintf(stderr, _("I was unable to create: \"%s\"!\n"), path);
		return;
	}

	for (i = 0; i < SCREEN_TABLE_MAX_PLAYERS; i++) {
		list_add(textFile->text, strdup("no_name 0"));
	}

	text_file_save(textFile);
}

int table_add(char *name, int score)
{
	int i;

	for (i = 0; i < SCREEN_TABLE_MAX_PLAYERS; i++) {
		char *line;
		char thisName[STR_NAME_SIZE];
		int thisCore;

		line = (char *) textFile->text->list[i];
		sscanf(line, "%s %d", thisName, &thisCore);

		if (score >= thisCore) {
			char new[STR_SIZE];

			sprintf(new, "%s %d", name, score);
			list_ins(textFile->text, i, strdup(new));
			list_del_item(textFile->text, SCREEN_TABLE_MAX_PLAYERS, free);
			setWidgetLabel();

			return 0;
		}
	}

	return -1;
}

void table_init()
{
	image_t *image;

	image = image_add("screen_table.png", IMAGE_NO_ALPHA, "screen_table", IMAGE_GROUP_BASE);
	image_backgorund = wid_image_new(0, 0, image);

	button_back = button_new(_("back"), WINDOW_SIZE_X / 2 - WIDGET_BUTTON_WIDTH / 2,
						 WINDOW_SIZE_Y - 100, eventWidget);

	loadHighscoreFile();
	listWidgetLabelNumer = NULL;
	listWidgetLabelName = NULL;
	listWidgetLabelScore = NULL;
	setWidgetLabel();

	screen_register(screen_new("table", table_start, table_event, table_draw, table_stop));
}

void table_quit()
{
	wid_image_destroy(image_backgorund);

	button_destroy(button_back);
	list_destroy_item(listWidgetLabelNumer, label_destroy);
	list_destroy_item(listWidgetLabelName, label_destroy);
	list_destroy_item(listWidgetLabelScore, label_destroy);

	if (textFile != NULL) {
		text_file_save(textFile);
		text_file_destroy(textFile);
	}
}
