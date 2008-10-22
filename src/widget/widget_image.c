
#include <stdlib.h>
#include <assert.h>

#include <stdlib.h>
#include "main.h"
#include "image.h"
#include "interface.h"
#include "font.h"

#include "widget.h"
#include "widget_image.h"

widget_t* newWidgetImage(int x, int y, image_t *image)
{
	widget_image_t *new;

	new = malloc( sizeof(widget_image_t) );
	new->image = image;

	return newWidget(WIDGET_TYPE_IMAGE, x, y, image->w, image->h, new);
}

void drawWidgetImage(widget_t *widget)
{
	widget_image_t *p;

	assert( widget != NULL );
	assert( widget->type == WIDGET_TYPE_IMAGE );

	p = (widget_image_t *) widget->private_data;
	drawImage(p->image, widget->x, widget->y, 0, 0, p->image->w, p->image->h);
}

void eventWidgetImage(widget_t *widget)
{
	assert( widget != NULL );
	assert( widget->type == WIDGET_TYPE_IMAGE );
}

void destroyWidgetImage(widget_t *widget)
{
	widget_image_t *p;

	assert( widget != NULL );
	assert( widget->type == WIDGET_TYPE_IMAGE );

	p = (widget_image_t *) widget->private_data;

	free(p);
	destroyWidget(widget);
}
