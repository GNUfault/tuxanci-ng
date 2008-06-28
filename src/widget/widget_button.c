
#include <stdlib.h>

#include "base/main.h"

#include "client/interface.h"
#include "client/image.h"
#include "client/font.h"

#include "widget_button.h"

widget_button_t* newWidgetButton(char *text, int x, int y, void (*fce_event)(void *))
{
	widget_button_t *new;

	new = malloc( sizeof(widget_button_t) );
	new->text = strdup(text);
	new->x = x;
	new->y = y;
	new->fce_event = fce_event;
	getTextSize(text, &new->w, &new->h);

	return new;
}

void drawWidgetButton(widget_button_t *p)
{
	static SDL_Surface *g_button0 = NULL;
	static SDL_Surface *g_button1 = NULL;
	int x, y;

	getMousePosition(&x, &y);

	if( g_button0 == NULL )
	{
		g_button0 = addImageData("button0.png", IMAGE_ALPHA, "button0", IMAGE_GROUP_BASE);
	}

	if( g_button1 == NULL )
	{
		g_button1 = addImageData("button1.png", IMAGE_ALPHA, "button1", IMAGE_GROUP_BASE);
	}

	if( x >= p->x && x <= p->x+WIDGET_BUTTON_WIDTH && 
	    y >= p->y && y <= p->y+WIDGET_BUTTON_HEIGHT )
	{
		drawImage(g_button1, p->x, p->y, 0, 0, g_button0->w, g_button0->h);
	}
	else
	{
		drawImage(g_button0, p->x, p->y, 0, 0, g_button0->w, g_button0->h);
	}

	//drawFont(p->text, p->x+WIDGET_BUTTON_WIDTH/2-p->w/2, p->y+p->h/2, COLOR_WHITE);
	drawFont(p->text, p->x + WIDGET_BUTTON_WIDTH/2 - p->w/2, p->y + WIDGET_BUTTON_HEIGHT/2 - p->h/2, COLOR_WHITE);
}

void eventWidgetButton(widget_button_t *p)
{
	static int time = 0;
	int x, y;

	if( time > 0)
	{
		time--;
		return;
	}

	getMousePosition(&x, &y);

	if( x >= p->x && x <= p->x+WIDGET_BUTTON_WIDTH && 
	    y >= p->y && y <= p->y+WIDGET_BUTTON_HEIGHT &&
	    isMouseClicked() )
	{
		time = WIDGET_BUTTON_TIME;
		p->fce_event(p);
	}
}

void destroyWidgetButton(widget_button_t *p)
{
	free(p->text);
	free(p);
}
