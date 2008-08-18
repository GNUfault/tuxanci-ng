
#include <stdio.h>
#include <assert.h>

#include "main.h"
#include "list.h"
#include "arenaFile.h"

#include "interface.h"
#include "screen.h"
#include "image.h"

#ifndef NO_SOUND
#include "music.h"
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

static list_t *listWidgetButtonimage;
static int choiceArenaId;

void startScreenChoiceArena()
{
#ifndef NO_SOUND
	playMusic("menu", MUSIC_GROUP_BASE);
#endif
}

void drawScreenChoiceArena()
{
	int i;

	drawWidgetImage(image_backgorund);

	drawWidgetButton(button_play);
	drawWidgetButton(button_back);

	for( i = 0 ; i < listWidgetButtonimage->count ; i++ )
	{
		widget_t *this;
		this = (widget_t *) listWidgetButtonimage->list[i];
		drawWidgetButtonimage(this);
	}
}

void eventScreenChoiceArena()
{
	int i;

	eventWidgetButton(button_play);
	eventWidgetButton(button_back);

	for( i = 0 ; i < listWidgetButtonimage->count ; i++ )
	{
		widget_t *this;
		this = (widget_t *) listWidgetButtonimage->list[i];
		eventWidgetButtonimage(this);
	}
}

void stopScreenChoiceArena()
{
}

static void eventWidgetButtonImage(void *p)
{
	widget_t *buttonimage;
	int i;

	buttonimage = (widget_t *)(p);

	for( i = 0 ; i < listWidgetButtonimage->count ; i++ )
	{
		widget_t *this;

		this = (widget_t *)(listWidgetButtonimage->list[i]);
		
		if( buttonimage == this )
		{
			setWidgetButtonimageActive(this, TRUE);
			choiceArenaId = i;
		}
		else
		{
			setWidgetButtonimageActive(this, FALSE);
		}
	}
}

int getChoiceArenaId()
{
	return choiceArenaId;
}

void setChoiceArenaId(int n)
{
	widget_t *widget_buttonimage;

	choiceArenaId = n;

	widget_buttonimage = (widget_t *)listWidgetButtonimage->list[choiceArenaId];
	setWidgetButtonimageActive(widget_buttonimage, TRUE);
}

static void eventWidget(void *p)
{
	widget_t *button;
	
	button = (widget_t *)(p);

	if( button == button_play )
	{
		setScreen("world");
	}

	if( button == button_back )
	{
		setScreen("gameType");
	}
}

void initScreenChoiceArena()
{
	image_t *image;
	int i;

	image = getImage(IMAGE_GROUP_BASE, "screen_main");
	image_backgorund  = newWidgetImage(0, 0, image);

	button_back = newWidgetButton(_("back"), 100, WINDOW_SIZE_Y-100, eventWidget);
	button_play = newWidgetButton(_("play"), WINDOW_SIZE_X-200, WINDOW_SIZE_Y-100, eventWidget);
	
	listWidgetButtonimage = newList();
	choiceArenaId = 0;

	for( i = 0 ; i < getArenaCount() ; i++ )
	{
		widget_t *widget_buttonimage;
		int x, y;

		image = addImageData(getArenaImage(i), IMAGE_NO_ALPHA, "none", IMAGE_GROUP_BASE);

		x = 100 + 200 * ( i -  3 * ( i/3 ) );
		y = 150 + 200 * ( i/3 );

		widget_buttonimage = newWidgetButtonimage(image,
			x, y, eventWidgetButtonImage);
/*
		if( i == choiceArenaId )
		{
			widget_buttonimage->active = 1;
		}
*/
		addList( listWidgetButtonimage, widget_buttonimage);
	}

	registerScreen( newScreen("chiceArena", startScreenChoiceArena, eventScreenChoiceArena,
		drawScreenChoiceArena, stopScreenChoiceArena) );
}

void quitScreenChoiceArena()
{
	destroyWidgetImage(image_backgorund);

	destroyWidgetButton(button_play);
	destroyWidgetButton(button_back);

	destroyListItem(listWidgetButtonimage, destroyWidgetButtonimage);
}
