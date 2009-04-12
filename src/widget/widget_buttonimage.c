
#include <stdlib.h>
#include <assert.h>

#include "main.h"
#include "interface.h"
#include "font.h"
#include "image.h"

#include "widget.h"
#include "widget_buttonimage.h"

widget_t *buttonImage_new(image_t * image, int x, int y, void (*fce_event) (void *))
{
	widget_buttonimage_t *new;

	new = malloc(sizeof(widget_buttonimage_t));
	new->image = image;
	new->w = image->w / 2;
	new->h = image->h;
	new->active = 0;
	new->fce_event = fce_event;

	return widget_new(WIDGET_TYPE_BUTTONIMAGE, x, y, new->w, new->h, new);
}

void buttonImage_set_active(widget_t * widget, bool_t active)
{
	widget_buttonimage_t *p;

	assert(widget != NULL);
	assert(widget->type == WIDGET_TYPE_BUTTONIMAGE);

	p = (widget_buttonimage_t *) widget->private_data;
	p->active = active;
}

void buttonImage_draw(widget_t * widget)
{
	widget_buttonimage_t *p;

	assert(widget != NULL);
	assert(widget->type == WIDGET_TYPE_BUTTONIMAGE);

	p = (widget_buttonimage_t *) widget->private_data;
	image_draw(p->image, widget->x, widget->y, p->active * p->w, 0, p->w, p->h);
}

void buttonImage_event(widget_t * widget)
{
	widget_buttonimage_t *p;
	static int my_time = 0;
	int x, y;

	assert(widget != NULL);
	assert(widget->type == WIDGET_TYPE_BUTTONIMAGE);

	p = (widget_buttonimage_t *) widget->private_data;

	if (my_time > 0) {
		my_time--;
		return;
	}

	interface_get_mouse_position(&x, &y);

	if (x >= widget->x && x <= widget->x + p->w &&
	    y >= widget->y && y <= widget->y + p->h && interface_is_mouse_clicket()) {
		my_time = WIDGET_BUTTONIMAGE_TIME;
		p->active = 1;
		p->fce_event(widget);
	}
}

void buttonImage_destroy(widget_t * widget)
{
	widget_buttonimage_t *p;

	assert(widget != NULL);
	assert(widget->type == WIDGET_TYPE_BUTTONIMAGE);

	p = (widget_buttonimage_t *) widget->private_data;

	free(p);
	widget_destroy(widget);
}
