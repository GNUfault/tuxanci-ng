
#include <stdio.h>
#include <assert.h>

#include "main.h"
#include "list.h"
#include "textFile.h"

#include "interface.h"
#include "screen.h"
#include "image.h"

#ifndef NO_SOUND
#include "music.h"
#endif

#include "screen_credits.h"

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

void startScreenCredits()
{
#ifndef NO_SOUND
	playMusic("menu", MUSIC_GROUP_BASE);
#endif
	offset = 0;
}

void drawScreenCredits()
{
	int i;

	drawWidgetImage(image_backgorund);

	drawWidgetButton(button_back);

	for( i = 0 ; i < listWidgetLabel->count ; i++ )
	{
		int z;

		widget_t *this;
		this = (widget_t *)listWidgetLabel->list[i];
		
		z = this->y;
		this->y += offset;

		if( this->y > SCREEN_CREDITS_OFFSET_MIN && this->y < SCREEN_CREDITS_OFFSET_MAX )
		{
			drawWidgetLabel(this);
		}

		this->y = z;
	}

}

void eventScreenCredits()
{
	eventWidgetButton(button_back);
	
	offset -= SCREEN_CREDITS_OFFSET_SPEED;

	if( offset < SCREEN_CREDITS_OFFSET_RESTART )
	{
		offset = 0;
	}

	//printf("offset = %d\n", offset);
}

void stopScreenCredits()
{
}

static void eventWidget(void *p)
{
	widget_t *button;
	
	button = (widget_t *)(p);

	if( button == button_back )
	{
		setScreen("mainMenu");
	}
}

void initScreenCredits()
{
	image_t *image;
	int i;

	image = getImage(IMAGE_GROUP_BASE, "screen_main");
	image_backgorund  = newWidgetImage(0, 0, image);

	button_back = newWidgetButton(_("back"), WINDOW_SIZE_X/2 -WIDGET_BUTTON_WIDTH/2, WINDOW_SIZE_Y-80, eventWidget);

	listWidgetLabel = newList();

	if( tryExistFile(PATH_DOC SCREEN_CREDITS_FILE) == 0 )
	{
		creditExists = 1;
		textFile = loadTextFile(PATH_DOC SCREEN_CREDITS_FILE);
    	for( i = 0 ; i < textFile->text->count ; i++ )
		{
			widget_t *label;
			char *line;
			line = (char *)textFile->text->list[i];
			label = newWidgetLabel(line, WINDOW_SIZE_X/2-WINDOW_SIZE_X/4,
				(WINDOW_SIZE_Y-100)+i*20, WIDGET_LABEL_LEFT);

			addList(listWidgetLabel, label);
		}
	}
	else
	{
		for( i = 0 ; i < 5 ; i++ )
		{
			creditExists = 0;
			widget_t *label;
			char line[STR_SIZE];
			sprintf(line, _("Credit file not found... %s/%s"),PATH_DOC,SCREEN_CREDITS_FILE);
			label = newWidgetLabel(line, WINDOW_SIZE_X/2, (WINDOW_SIZE_Y-100)+i*20,
				WIDGET_LABEL_CENTER);

			addList(listWidgetLabel, label);
		}
	}
	
	registerScreen( newScreen("credits", startScreenCredits, eventScreenCredits,
		drawScreenCredits, stopScreenCredits) );
}

void quitScreenCredits()
{
	destroyWidgetImage(image_backgorund);

	destroyWidgetButton(button_back);
	destroyListItem(listWidgetLabel, destroyWidgetLabel);
	if( creditExists )
	{
		destroyTextFile(textFile);
	}
}

