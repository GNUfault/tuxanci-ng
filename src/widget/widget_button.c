#include <stdlib.h>
#include <assert.h>

#include "main.h"

#include "interface.h"
#include "image.h"
#include "font.h"

#include "widget.h"
#include "widget_button.h"

widget_t *button_new(char *text, int x, int y, void (*fce_event) (void *))
{
	widget_button_t *new;

	new = malloc(sizeof(widget_button_t));
	new->text = strdup(text);
	new->fce_event = fce_event;
	font_text_size(text, &(new->w), &(new->h));

	return widget_new(WIDGET_TYPE_BUTTON, x, y,
			  WIDGET_BUTTON_WIDTH, WIDGET_BUTTON_HEIGHT, new);
}

void button_draw(widget_t *widget)
{
	widget_button_t *p;
	static image_t *g_button0 = NULL;
	static image_t *g_button1 = NULL;
	int x, y;

	assert(widget != NULL);
	assert(widget->type == WIDGET_TYPE_BUTTON);

	p = (widget_button_t *) widget->private_data;
	interface_get_mouse_position(&x, &y);

	if (g_button0 == NULL) {
		g_button0 = image_add("button0.png", IMAGE_ALPHA, "button0", IMAGE_GROUP_BASE);
	}

	if (g_button1 == NULL) {
		g_button1 = image_add("button1.png", IMAGE_ALPHA, "button1", IMAGE_GROUP_BASE);
	}

	if (x >= widget->x && x <= widget->x + WIDGET_BUTTON_WIDTH &&
	    y >= widget->y && y <= widget->y + WIDGET_BUTTON_HEIGHT) {
		image_draw(g_button1, widget->x, widget->y, 0, 0, g_button0->w, g_button0->h);
	} else {
		image_draw(g_button0, widget->x, widget->y, 0, 0, g_button0->w, g_button0->h);
	}

	/*font_draw(p->text, p->x+WIDGET_BUTTON_WIDTH/2-p->w/2, p->y+p->h/2, COLOR_WHITE);*/
	font_draw(p->text, widget->x + WIDGET_BUTTON_WIDTH / 2 - p->w / 2,
			   widget->y + WIDGET_BUTTON_HEIGHT / 2 - p->h / 2,
			   COLOR_WHITE);
}

void button_event(widget_t *widget)
{
	widget_button_t *p;
	static int my_time = 0;
	int x, y;

	assert(widget != NULL);
	assert(widget->type == WIDGET_TYPE_BUTTON);

	p = (widget_button_t *) widget->private_data;

	if (my_time > 0) {
		my_time--;
		return;
	}

	interface_get_mouse_position(&x, &y);

	if (x >= widget->x && x <= widget->x + WIDGET_BUTTON_WIDTH &&
	    y >= widget->y && y <= widget->y + WIDGET_BUTTON_HEIGHT &&
	    interface_is_mouse_clicket()) {
		my_time = WIDGET_BUTTON_TIME;

		DEBUG_MSG(_("[Debug] Caught some button event\n"));

		p->fce_event(widget);
	}
}

void button_destroy(widget_t *widget)
{
	widget_button_t *p;

	assert(widget != NULL);
	assert(widget->type == WIDGET_TYPE_BUTTON);

	p = (widget_button_t *) widget->private_data;

	free(p->text);
	free(p);

	widget_destroy(widget);
}
