
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

#include "screen_mainMenu.h"
#include "screen_analyze.h"

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
	setScreen("mainMenu");
}

void startScreenAnalyze()
{
#ifndef NO_SOUND
	playMusic("menu", MUSIC_GROUP_BASE);
#endif

	registerHotKey(SDLK_ESCAPE, hotkey_escape);
}

void drawScreenAnalyze()
{
	widget_t *this;
	int i;

	drawWidgetImage(image_backgorund);

	for (i = 0; i < listWidgetLabelName->count; i++) {
		this = (widget_t *) (listWidgetLabelName->list[i]);
		drawWidgetLabel(this);
	}

	for (i = 0; i < listWidgetLabelScore->count; i++) {
		this = (widget_t *) (listWidgetLabelScore->list[i]);
		drawWidgetLabel(this);
	}

	drawWidgetLabel(widgetLabelMsg);
	drawWidgetButton(button_ok);
}

void eventScreenAnalyze()
{
	eventWidgetButton(button_ok);
}

void stopScreenAnalyze()
{
	destroyWidgetLabel(widgetLabelMsg);
	widgetLabelMsg = newWidgetLabel("", WINDOW_SIZE_X / 2, 250, WIDGET_LABEL_CENTER);

	unregisterHotKey(SDLK_ESCAPE);
}

static void eventWidget(void *p)
{
	widget_t *button;

	button = (widget_t *) (p);

	if (button == button_ok) {
		setScreen("mainMenu");
	}
}

void restartAnalyze()
{
	destroyListItem(listWidgetLabelName, destroyWidgetLabel);
	destroyListItem(listWidgetLabelScore, destroyWidgetLabel);
	destroyListItem(listAnalyze, destroyAnalyze);

	listWidgetLabelName = newList();
	listWidgetLabelScore = newList();
	listAnalyze = newList();
}

void addAnalyze(char *name, int score)
{
	addList(listAnalyze, newAnalyze(name, score));
}

void setMsgToAnalyze(char *msg)
{
	destroyWidgetLabel(widgetLabelMsg);
	widgetLabelMsg = newWidgetLabel(msg, WINDOW_SIZE_X / 2, 250, WIDGET_LABEL_CENTER);
}

void endAnalyze()
{
	int i;

	for (i = 0; i < listAnalyze->count; i++) {
		analyze_t *this;
		char *str;

		this = (analyze_t *) (listAnalyze->list[i]);

		addList(listWidgetLabelName, newWidgetLabel(this->name, 100,
							    200 + 20 * i, WIDGET_LABEL_LEFT));

		str = getString(this->score);

		addList(listWidgetLabelScore, newWidgetLabel(str, WINDOW_SIZE_X - 100,
							     200 + 20 * i, WIDGET_LABEL_RIGHT));

		free(str);
	}
}

void initScreenAnalyze()
{
	image_t *image;

	image = getImage(IMAGE_GROUP_BASE, "screen_main");
	image_backgorund = newWidgetImage(0, 0, image);

	listWidgetLabelName = newList();
	listWidgetLabelScore = newList();
	listAnalyze = newList();

	button_ok = newWidgetButton("OK", WINDOW_SIZE_X / 2 - WIDGET_BUTTON_WIDTH / 2,
				    WINDOW_SIZE_Y - 100, eventWidget);

	registerScreen( newScreen("analyze", startScreenAnalyze, eventScreenAnalyze,
			drawScreenAnalyze, stopScreenAnalyze));

	widgetLabelMsg = newWidgetLabel("", WINDOW_SIZE_X / 2, 250, WIDGET_LABEL_CENTER);
}

void quitScreenAnalyze()
{
	destroyWidgetImage(image_backgorund);

	destroyListItem(listWidgetLabelName, destroyWidgetLabel);
	destroyListItem(listWidgetLabelScore, destroyWidgetLabel);
	destroyListItem(listAnalyze, destroyAnalyze);

	destroyWidgetButton(button_ok);
	destroyWidgetLabel(widgetLabelMsg);
}
