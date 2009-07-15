#include <stdlib.h>
#include <assert.h>

#include "main.h"
#include "interface.h"
#include "font.h"
#include "image.h"
#include "mouse_buffer.h"

#include "widget.h"
#include "widget_buttonimage.h"

widget_t *button_image_new(image_t *image, int x, int y, void (*fce_event) (void *))
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

void button_image_set_active(widget_t *widget, bool_t active)
{
	widget_buttonimage_t *p;

	assert(widget != NULL);
	assert(widget->type == WIDGET_TYPE_BUTTONIMAGE);

	p = (widget_buttonimage_t *) widget->private_data;
	p->active = active;
}

void button_image_draw(widget_t *widget)
{
	widget_buttonimage_t *p;

	assert(widget != NULL);
	assert(widget->type == WIDGET_TYPE_BUTTONIMAGE);

	p = (widget_buttonimage_t *) widget->private_data;
	image_draw(p->image, widget->x, widget->y, p->active *p->w, 0, p->w, p->h);
}

void button_image_event(widget_t *widget)
{
	widget_buttonimage_t *p;
	static int my_time = 0;

	assert(widget != NULL);
	assert(widget->type == WIDGET_TYPE_BUTTONIMAGE);

	p = (widget_buttonimage_t *) widget->private_data;

	if (my_time > 0) {
		my_time--;
		return;
	}

	if (mouse_buffer_is_on_area(widget->x, widget->y,  p->w, p->h, MOUSE_BUF_CLICK)) {
		my_time = WIDGET_BUTTONIMAGE_TIME;
		p->active = 1;
		p->fce_event(widget);
	}
}

void button_image_destroy(widget_t *widget)
{
	widget_buttonimage_t *p;

	assert(widget != NULL);
	assert(widget->type == WIDGET_TYPE_BUTTONIMAGE);

	p = (widget_buttonimage_t *) widget->private_data;

	free(p);
	widget_destroy(widget);
}
