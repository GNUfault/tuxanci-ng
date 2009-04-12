
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "main.h"
#include "list.h"

#include "interface.h"
#include "screen.h"
#include "image.h"
#include "hotKey.h"

#ifndef NO_SOUND
#    include "music.h"
#endif

#include "mainMenu.h"
#include "analyze.h"

#include "widget.h"
#include "widget_label.h"
#include "widget_button.h"
#include "widget_image.h"

static widget_t *image_backgorund;

static list_t *listWidgetLabelName;
static list_t *listWidgetLabelScore;
static list_t *listAnalyze;
static widget_t *widgetLabelMsg;

static widget_t *button_ok;

static analyze_t *newAnalyze(char *name, int score)
{
	analyze_t *new;

	new = malloc(sizeof(analyze_t));
	new->name = strdup(name);
	new->score = score;

	return new;
}

static void destroyAnalyze(analyze_t * p)
{
	free(p->name);
	free(p);
}

static void hotkey_escape()
{
	screen_set("mainMenu");
}

void analyze_start()
{
#ifndef NO_SOUND
	music_play("menu", MUSIC_GROUP_BASE);
#endif

	hotKey_register(SDLK_ESCAPE, hotkey_escape);
}

void analyze_draw()
{
	widget_t *this;
	int i;

	wid_image_draw(image_backgorund);

	for (i = 0; i < listWidgetLabelName->count; i++) {
		this = (widget_t *) (listWidgetLabelName->list[i]);
		label_draw(this);
	}

	for (i = 0; i < listWidgetLabelScore->count; i++) {
		this = (widget_t *) (listWidgetLabelScore->list[i]);
		label_draw(this);
	}

	label_draw(widgetLabelMsg);
	button_draw(button_ok);
}

void analyze_event()
{
	button_event(button_ok);
}

void analyze_stop()
{
	label_destroy(widgetLabelMsg);
	widgetLabelMsg = label_new("", WINDOW_SIZE_X / 2, 250, WIDGET_LABEL_CENTER);

	unhotKey_register(SDLK_ESCAPE);
}

static void eventWidget(void *p)
{
	widget_t *button;

	button = (widget_t *) (p);

	if (button == button_ok) {
		screen_set("mainMenu");
	}
}

void analyze_restart()
{
	list_destroy_item(listWidgetLabelName, label_destroy);
	list_destroy_item(listWidgetLabelScore, label_destroy);
	list_destroy_item(listAnalyze, destroyAnalyze);

	listWidgetLabelName = list_new();
	listWidgetLabelScore = list_new();
	listAnalyze = list_new();
}

void analyze_add(char *name, int score)
{
	list_add(listAnalyze, newAnalyze(name, score));
}

void analyze_set_msg(char *msg)
{
	label_destroy(widgetLabelMsg);
	widgetLabelMsg = label_new(msg, WINDOW_SIZE_X / 2, 250, WIDGET_LABEL_CENTER);
}

void analyze_end()
{
	int i;

	for (i = 0; i < listAnalyze->count; i++) {
		analyze_t *this;
		char *str;

		this = (analyze_t *) (listAnalyze->list[i]);

		list_add(listWidgetLabelName, label_new(this->name, 100,
							    200 + 20 * i, WIDGET_LABEL_LEFT));

		str = getString(this->score);

		list_add(listWidgetLabelScore, label_new(str, WINDOW_SIZE_X - 100,
							     200 + 20 * i, WIDGET_LABEL_RIGHT));

		free(str);
	}
}

void analyze_init()
{
	image_t *image;

	image = image_get(IMAGE_GROUP_BASE, "screen_main");
	image_backgorund = wid_image_new(0, 0, image);

	listWidgetLabelName = list_new();
	listWidgetLabelScore = list_new();
	listAnalyze = list_new();

	button_ok = button_new("OK", WINDOW_SIZE_X / 2 - WIDGET_BUTTON_WIDTH / 2,
				    WINDOW_SIZE_Y - 100, eventWidget);

	screen_register( screen_new("analyze", analyze_start, analyze_event,
			analyze_draw, analyze_stop));

	widgetLabelMsg = label_new("", WINDOW_SIZE_X / 2, 250, WIDGET_LABEL_CENTER);
}

void analyze_quit()
{
	wid_image_destroy(image_backgorund);

	list_destroy_item(listWidgetLabelName, label_destroy);
	list_destroy_item(listWidgetLabelScore, label_destroy);
	list_destroy_item(listAnalyze, destroyAnalyze);

	button_destroy(button_ok);
	label_destroy(widgetLabelMsg);
}
