
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "main.h"
#include "language.h"
#include "interface.h"
#include "list.h"
#include "textFile.h"
#include "homeDirector.h"
#include "screen.h"
#include "image.h"
#include "music.h"
#include "fake_audio.h"

#include "screen_table.h"

#include "widget_image.h"
#include "widget_label.h"
#include "widget_button.h"

static widget_image_t *image_backgorund;
static widget_button_t *button_back;

static list_t *listWidgetLabelNumer;
static list_t *listWidgetLabelName;
static list_t *listWidgetLabelScore;

static textFile_t *textFile;

void startScreenTable()
{
	playMusic("menu", MUSIC_GROUP_BASE);
}

void drawScreenTable()
{
	int i;

	drawWidgetImage(image_backgorund);

	drawWidgetButton(button_back);

	for( i = 0 ; i < listWidgetLabelNumer->count ; i++ )
	{
		widget_label_t *this;
		this = (widget_label_t *)listWidgetLabelNumer->list[i];
		drawWidgetLabel(this);
	}

	for( i = 0 ; i < listWidgetLabelName->count ; i++ )
	{
		widget_label_t *this;
		this = (widget_label_t *)listWidgetLabelName->list[i];
		drawWidgetLabel(this);
	}

	for( i = 0 ; i < listWidgetLabelScore->count ; i++ )
	{
		widget_label_t *this;
		this = (widget_label_t *)listWidgetLabelScore->list[i];
		drawWidgetLabel(this);
	}
}

void eventScreenTable()
{
	eventWidgetButton(button_back);
}

void stopScreenTable()
{
}

static void eventWidget(void *p)
{
	widget_button_t *button;
	
	button = (widget_button_t *)(p);

	if( button == button_back )
	{
		setScreen("mainMenu");
	}
}

static void setWidgetLabel()
{
	int i;

	if( textFile == NULL )
	{
		fprintf(stderr, "File %s not loaded !",
			SCREEN_TABLE_FILE_HIGHSCORE_NAME);
		
		return;
	}

	if( listWidgetLabelNumer != NULL ||
	    listWidgetLabelName != NULL ||
	    listWidgetLabelScore != NULL )
	{
		destroyListItem(listWidgetLabelNumer, destroyWidgetLabel);
		destroyListItem(listWidgetLabelName, destroyWidgetLabel);
		destroyListItem(listWidgetLabelScore, destroyWidgetLabel);
	}

	listWidgetLabelNumer = newList();
	listWidgetLabelName = newList();
	listWidgetLabelScore = newList();

	for( i = 0 ; i < SCREEN_TABLE_MAX_PLAYERS ; i++ )
	{
		widget_label_t *label;
		char name[STR_NAME_SIZE];
		char score[STR_NUM_SIZE];
		char num[STR_NUM_SIZE];
		char *line;

		line = (char *) textFile->text->list[i];

		sscanf(line, "%s %s", name, score);
		sprintf(num, "%2d)", i+1);

		label = newWidgetLabel(num, WINDOW_SIZE_X/2 - 100, 200 + i * 20, WIDGET_LABEL_LEFT);
		addList(listWidgetLabelNumer, label);

		label = newWidgetLabel(name, WINDOW_SIZE_X/2, 200 + i * 20, WIDGET_LABEL_CENTER);
		addList(listWidgetLabelName, label);

		label = newWidgetLabel(score, WINDOW_SIZE_X/2 + 80, 200 + i * 20, WIDGET_LABEL_LEFT);
		addList(listWidgetLabelScore, label);
	}
}

static void loadHighscoreFile()
{
	char path[STR_PATH_SIZE];
	int i;

	sprintf(path, "%s/" SCREEN_TABLE_FILE_HIGHSCORE_NAME , getHomeDirector());
	textFile = loadTextFile(path);

	if( textFile == NULL )
	{
		fprintf(stderr, "I can't load %s !\n", path);
		fprintf(stderr, "I create %s\n", path);
		textFile = newTextFile(path);
	}
	else
	{
		return;
	}

	if( textFile == NULL )
	{
		fprintf(stderr, "I can't create %s !\n", path);
		return;
	}

	for( i = 0 ; i < SCREEN_TABLE_MAX_PLAYERS ; i++ )
	{
		addList(textFile->text, strdup("no_name 0") );
	}

	saveTextFile(textFile);
}

int addPlayerInHighScore(char *name, int score)
{
	int i;

	for( i = 0 ; i < SCREEN_TABLE_MAX_PLAYERS ; i++ )
	{
		char *line;
		char thisName[STR_NAME_SIZE];
		int thisCore;

		line = (char *) textFile->text->list[i];
		sscanf(line, "%s %d", thisName, &thisCore);

		if( score >= thisCore )
		{
			char new[STR_SIZE];
			
			sprintf(new, "%s %d", name, score);
			insList(textFile->text, i, strdup(new) );
			delListItem(textFile->text, SCREEN_TABLE_MAX_PLAYERS, free);
			setWidgetLabel();

			return 0;
		}
	}

	return -1;
}

void initScreenTable()
{
	SDL_Surface *image;


	image = addImageData("screen_table.png", IMAGE_NO_ALPHA, "screen_table", IMAGE_GROUP_BASE);
	image_backgorund  = newWidgetImage(0, 0, image);

	button_back = newWidgetButton(getMyText("BACK"), WINDOW_SIZE_X/2 -WIDGET_BUTTON_WIDTH/2,
		WINDOW_SIZE_Y-100, eventWidget);

	loadHighscoreFile();
	listWidgetLabelNumer = NULL;
	listWidgetLabelName = NULL;
	listWidgetLabelScore = NULL;
	setWidgetLabel();

	registerScreen( newScreen("table", startScreenTable, eventScreenTable,
		drawScreenTable, stopScreenTable) );
}

void quitScreenTable()
{
	destroyWidgetImage(image_backgorund);

	destroyWidgetButton(button_back);
	destroyListItem(listWidgetLabelNumer, destroyWidgetLabel);
	destroyListItem(listWidgetLabelName, destroyWidgetLabel);
	destroyListItem(listWidgetLabelScore, destroyWidgetLabel);
	
	if( textFile != NULL )
	{
		saveTextFile(textFile);
		destroyTextFile(textFile);
	}
}

