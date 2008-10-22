
#include <stdlib.h>
#include <assert.h>

#include "main.h"
#include "interface.h"
#include "font.h"
#include "image.h"

#include "widget.h"
#include "widget_buttonimage.h"

widget_t* newWidgetButtonimage(image_t *image, int x, int y, void (*fce_event)(void *))
{
	widget_buttonimage_t *new;

	new = malloc( sizeof(widget_buttonimage_t) );
	new->image = image;
	new->w = image->w/2;
	new->h = image->h;
	new->active = 0;
	new->fce_event = fce_event;

	return newWidget(WIDGET_TYPE_BUTTONIMAGE, x, y, new->w, new->h, new);
}

void setWidgetButtonimageActive(widget_t *widget, bool_t active)
{
	widget_buttonimage_t *p;

	assert( widget != NULL );
	assert( widget->type == WIDGET_TYPE_BUTTONIMAGE );

	p = (widget_buttonimage_t *)widget->private_data;
 	p->active = active;
}

void drawWidgetButtonimage(widget_t *widget)
{
	widget_buttonimage_t *p;

	assert( widget != NULL );
	assert( widget->type == WIDGET_TYPE_BUTTONIMAGE );

	p = (widget_buttonimage_t *)widget->private_data;
 	drawImage(p->image, widget->x, widget->y, p->active*p->w, 0, p->w, p->h);
}

void eventWidgetButtonimage(widget_t *widget)
{
	widget_buttonimage_t *p;
	static int time = 0;
	int x, y;

	assert( widget != NULL );
	assert( widget->type == WIDGET_TYPE_BUTTONIMAGE );

	p = (widget_buttonimage_t *) widget->private_data;
	if( time > 0)
	{
		time--;
		return;
	}

	getMousePosition(&x, &y);

	if( x >= widget->x && x <= widget->x+p->w && 
	    y >= widget->y && y <= widget->y+p->h &&
	    isMouseClicked() )
	{
		time = WIDGET_BUTTONIMAGE_TIME;
		p->active = 1;
		p->fce_event(widget);
	}
}

void destroyWidgetButtonimage(widget_t *widget)
{
	widget_buttonimage_t *p;

	assert( widget != NULL );
	assert( widget->type == WIDGET_TYPE_BUTTONIMAGE );

	p = (widget_buttonimage_t *) widget->private_data;

	free(p);
	destroyWidget(widget);
}
