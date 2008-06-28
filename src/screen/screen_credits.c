
#include <stdio.h>
#include <assert.h>

#include "base/main.h"
#include "base/list.h"
#include "base/textFile.h"

#include "client/language.h"
#include "client/interface.h"
#include "client/screen.h"
#include "client/image.h"

#ifndef NO_SOUND
#include "audio/music.h"
#endif

#include "screen/screen_credits.h"

#include "widget/widget_image.h"
#include "widget/widget_label.h"
#include "widget/widget_button.h"

static widget_image_t *image_backgorund;
static widget_button_t *button_back;
static list_t *listWidgetLabel;
static textFile_t *textFile;
static int offset;

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

		widget_label_t *this;
		this = (widget_label_t *)listWidgetLabel->list[i];
		
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
	widget_button_t *button;
	
	button = (widget_button_t *)(p);

	if( button == button_back )
	{
		setScreen("mainMenu");
	}
}

void initScreenCredits()
{
	SDL_Surface *image;
	int i;

	image = getImage(IMAGE_GROUP_BASE, "screen_main");
	image_backgorund  = newWidgetImage(0, 0, image);

	button_back = newWidgetButton(getMyText("BACK"), WINDOW_SIZE_X/2 -WIDGET_BUTTON_WIDTH/2, WINDOW_SIZE_Y-80, eventWidget);

	listWidgetLabel = newList();
	accessExistFile(PATH_DATA SCREEN_CREDITS_FILE);
	textFile = loadTextFile(PATH_DATA SCREEN_CREDITS_FILE);

	for( i = 0 ; i < textFile->text->count ; i++ )
	{
		widget_label_t *label;
		char *line;

		line = (char *)textFile->text->list[i];
		label = newWidgetLabel(line, WINDOW_SIZE_X/2, (WINDOW_SIZE_Y-100)+i*20,
			 WIDGET_LABEL_CENTER);

		addList(listWidgetLabel, label);
	}

	registerScreen( newScreen("credits", startScreenCredits, eventScreenCredits,
		drawScreenCredits, stopScreenCredits) );
}

void quitScreenCredits()
{
	destroyWidgetImage(image_backgorund);

	destroyWidgetButton(button_back);
	destroyListItem(listWidgetLabel, destroyWidgetLabel);
	destroyTextFile(textFile);
}

