
#include <stdlib.h>
#include <assert.h>

#include "main.h"
#include "interface.h"
#include "font.h"

#include "widget.h"
#include "widget_select.h"

widget_t *newWidgetSelect(int x, int y, void (*fce_event) (void *))
{
	widget_select_t *new;

	new = malloc(sizeof(widget_select_t));
	new->select = -1;
	new->fce_event = fce_event;
	new->list = newList();

	return newWidget(WIDGET_TYPE_SELECT, x, y, 0, 0, new);
}

char *getWidgetSelectItem(widget_t * widget)
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

int getWidgetSelectIndex(widget_t * widget)
{
	widget_select_t *p;

	assert(widget != NULL);
	assert(widget->type == WIDGET_TYPE_SELECT);

	p = (widget_select_t *) widget->private_data;

	return p->select;
}

void addToWidgetSelect(widget_t * widget, char *s)
{
	widget_select_t *p;

	assert(widget != NULL);
	assert(widget->type == WIDGET_TYPE_SELECT);

	p = (widget_select_t *) widget->private_data;

	addList(p->list, strdup(s));
}

void removeAllFromWidgetSelect(widget_t * widget)
{
	widget_select_t *p;

	assert(widget != NULL);
	assert(widget->type == WIDGET_TYPE_SELECT);

	p = (widget_select_t *) widget->private_data;

	while (p->list->count > 0) {
		delListItem(p->list, 0, free);
	}
}

void drawWidgetSelect(widget_t * widget)
{
	widget_select_t *p;
	int x, y, w, h;
	int i;

	assert(widget != NULL);
	assert(widget->type == WIDGET_TYPE_SELECT);

	getMousePosition(&x, &y);
	p = (widget_select_t *) widget->private_data;

	for (i = 0; i < p->list->count; i++) {
		char *line;

		line = (char *) p->list->list[i];

		getTextSize(line, &w, &h);

		if (x > widget->x && y > widget->y + i * 20 &&
		    x < widget->x + w && y < widget->y + i * 20 + h) {
			drawFont(line, widget->x, widget->y + i * 20, COLOR_YELLOW);
		} else {
			if (p->select == i) {
				drawFont(line, widget->x, widget->y + i * 20, COLOR_RED);
			} else {
				drawFont(line, widget->x, widget->y + i * 20, COLOR_WHITE);
			}
		}
	}
}

void eventWidgetSelect(widget_t * widget)
{
	widget_select_t *p;
	int x, y, w, h;
	int i;

	assert(widget != NULL);
	assert(widget->type == WIDGET_TYPE_SELECT);

	p = (widget_select_t *) widget->private_data;
	getMousePosition(&x, &y);

	for (i = 0; i < p->list->count; i++) {
		char *line;

		line = (char *) p->list->list[i];

		getTextSize(line, &w, &h);

		if (x > widget->x && y > widget->y + i * 20 &&
		    x < widget->x + w && y < widget->y + i * 20 + h &&
		    isMouseClicked()) {
			p->select = i;
			p->fce_event(p);
		}
	}
}

void destroyWidgetSelect(widget_t * widget)
{
	widget_select_t *p;

	assert(widget != NULL);
	assert(widget->type == WIDGET_TYPE_SELECT);

	p = (widget_select_t *) widget->private_data;

	destroyListItem(p->list, free);
	free(p);
	destroyWidget(widget);
}
