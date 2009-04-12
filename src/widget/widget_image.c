
#include <stdlib.h>
#include <assert.h>

#include <stdlib.h>
#include "main.h"
#include "image.h"
#include "interface.h"
#include "font.h"

#include "widget.h"
#include "widget_image.h"

widget_t *wid_image_new(int x, int y, image_t * image)
{
	widget_image_t *new;

	new = malloc(sizeof(widget_image_t));
	new->image = image;

	return widget_new(WIDGET_TYPE_IMAGE, x, y, image->w, image->h, new);
}

void wid_image_draw(widget_t * widget)
{
	widget_image_t *p;

	assert(widget != NULL);
	assert(widget->type == WIDGET_TYPE_IMAGE);

	p = (widget_image_t *) widget->private_data;
	image_draw(p->image, widget->x, widget->y, 0, 0, p->image->w, p->image->h);
}

void wid_image_event(widget_t * widget)
{
	assert(widget != NULL);
	assert(widget->type == WIDGET_TYPE_IMAGE);
}

void wid_image_destroy(widget_t * widget)
{
	widget_image_t *p;

	assert(widget != NULL);
	assert(widget->type == WIDGET_TYPE_IMAGE);

	p = (widget_image_t *) widget->private_data;

	free(p);
	widget_destroy(widget);
}
