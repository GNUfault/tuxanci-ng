
#include <stdlib.h>
#include <assert.h>

#include "main.h"
#include "interface.h"
#include "font.h"

#include "widget.h"
#include "widget_select.h"

widget_t *select_new(int x, int y, void (*fce_event) (void *))
{
	widget_select_t *new;

	new = malloc(sizeof(widget_select_t));
	new->select = -1;
	new->fce_event = fce_event;
	new->list = list_new();

	return widget_new(WIDGET_TYPE_SELECT, x, y, 0, 0, new);
}

char *select_get_item(widget_t * widget)
{
	widget_select_t *p;

	assert(widget != NULL);
	assert(widget->type == WIDGET_TYPE_SELECT);

	p = (widget_select_t *) widget->private_data;

	if (p->select == -1) {
		return NULL;
	}

	return (char *) p->list->list[p->select];
}

int select_get_index(widget_t * widget)
{
	widget_select_t *p;

	assert(widget != NULL);
	assert(widget->type == WIDGET_TYPE_SELECT);

	p = (widget_select_t *) widget->private_data;

	return p->select;
}

void select_add(widget_t * widget, char *s)
{
	widget_select_t *p;

	assert(widget != NULL);
	assert(widget->type == WIDGET_TYPE_SELECT);

	p = (widget_select_t *) widget->private_data;

	list_add(p->list, strdup(s));
}

void select_remove_all(widget_t * widget)
{
	widget_select_t *p;

	assert(widget != NULL);
	assert(widget->type == WIDGET_TYPE_SELECT);

	p = (widget_select_t *) widget->private_data;

	while (p->list->count > 0) {
		list_del_item(p->list, 0, free);
	}
}

void select_draw(widget_t * widget)
{
	widget_select_t *p;
	int x, y, w, h;
	int i;

	assert(widget != NULL);
	assert(widget->type == WIDGET_TYPE_SELECT);

	interface_get_mouse_position(&x, &y);
	p = (widget_select_t *) widget->private_data;

	for (i = 0; i < p->list->count; i++) {
		char *line;

		line = (char *) p->list->list[i];

		font_text_size(line, &w, &h);

		if (x > widget->x && y > widget->y + i * 20 &&
		    x < widget->x + w && y < widget->y + i * 20 + h) {
			font_draw(line, widget->x, widget->y + i * 20, COLOR_YELLOW);
		} else {
			if (p->select == i) {
				font_draw(line, widget->x, widget->y + i * 20, COLOR_RED);
			} else {
				font_draw(line, widget->x, widget->y + i * 20, COLOR_WHITE);
			}
		}
	}
}

void select_event(widget_t * widget)
{
	widget_select_t *p;
	int x, y, w, h;
	int i;

	assert(widget != NULL);
	assert(widget->type == WIDGET_TYPE_SELECT);

	p = (widget_select_t *) widget->private_data;
	interface_get_mouse_position(&x, &y);

	for (i = 0; i < p->list->count; i++) {
		char *line;

		line = (char *) p->list->list[i];

		font_text_size(line, &w, &h);

		if (x > widget->x && y > widget->y + i * 20 &&
		    x < widget->x + w && y < widget->y + i * 20 + h &&
		    interface_is_mouse_clicket()) {
			p->select = i;
			p->fce_event(p);
		}
	}
}

void select_destroy(widget_t * widget)
{
	widget_select_t *p;

	assert(widget != NULL);
	assert(widget->type == WIDGET_TYPE_SELECT);

	p = (widget_select_t *) widget->private_data;

	list_destroy_item(p->list, free);
	free(p);
	widget_destroy(widget);
}
