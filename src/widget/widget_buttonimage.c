
#include <stdlib.h>
#include "main.h"
#include "interface.h"
#include "font.h"
#include "image.h"
#include "widget_buttonimage.h"

widget_buttonimage_t* newWidgetButtonimage(image_t *image, int x, int y, void (*fce_event)(void *))
{
	widget_buttonimage_t *new;

	new = malloc( sizeof(widget_buttonimage_t) );
	new->image = image;
	new->x = x;
	new->y = y;
	new->w = image->w/2;
	new->h = image->h;
	new->active = 0;
	new->fce_event = fce_event;

	return new;
}

void drawWidgetButtonimage(widget_buttonimage_t *p)
{
 	drawImage(p->image, p->x, p->y, p->active*p->w, 0, p->w, p->h);
}

void eventWidgetButtonimage(widget_buttonimage_t *p)
{
	static int time = 0;
	int x, y;

	if( time > 0)
	{
		time--;
		return;
	}

	getMousePosition(&x, &y);

	if( x >= p->x && x <= p->x+p->w && 
	    y >= p->y && y <= p->y+p->h &&
	    isMouseClicked() )
	{
		time = WIDGET_BUTTONIMAGE_TIME;
		p->fce_event(p);
	}
}

void destroyWidgetButtonimage(widget_buttonimage_t *p)
{
	free(p);
}
