#include <stdlib.h>
#include <assert.h>

#include "main.h"
#include "interface.h"
#include "font.h"
#include "widget_label.h"
#include "widget.h"

widget_t *label_new(char *text, int x, int y, int bind)
{
	widget_label_t *new;

	new = malloc(sizeof(widget_label_t));
	new->text = strdup(text);
	new->bind = bind;
	font_text_size(text, &(new->w), &(new->h));

	return widget_new(WIDGET_TYPE_LABEL, x, y, new->w, new->h, new);
}

void label_draw(widget_t *widget)
{
	widget_label_t *p;

	assert(widget != NULL);
	assert(widget->type == WIDGET_TYPE_LABEL);

	p = (widget_label_t *) widget->private_data;

	switch (p->bind) {
		case WIDGET_LABEL_RIGHT:
			font_draw(p->text, widget->x - p->w, widget->y + p->h / 2, COLOR_WHITE);
			break;
		case WIDGET_LABEL_LEFT:
			font_draw(p->text, widget->x, widget->y + widget->h / 2, COLOR_WHITE);
			break;
		case WIDGET_LABEL_CENTER:
			font_draw(p->text, widget->x - p->w / 2, widget->y + p->h / 2, COLOR_WHITE);
			break;
		default:
			assert(!_("[Error] Unknown dimension for label placement"));
			break;
	}
}

void label_event(widget_t *widget)
{
	assert(widget != NULL);
	assert(widget->type == WIDGET_TYPE_LABEL);
}

void label_destroy(widget_t *widget)
{
	widget_label_t *p;

	assert(widget != NULL);
	assert(widget->type == WIDGET_TYPE_LABEL);

	p = (widget_label_t *) widget->private_data;

	free(p->text);
	free(p);

	widget_destroy(widget);
}
