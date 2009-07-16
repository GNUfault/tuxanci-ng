#include <stdlib.h>
#include <assert.h>

#include "main.h"
#include "interface.h"
#include "image.h"
#include "font.h"
#include "widget_statusbar.h"
#include "widget.h"
#include "mouse_buffer.h"

typedef struct item_struct {
	widget_t *widget;
	list_t *text;
} item_t;

static item_t *item_new(widget_t *widget, char *text)
{
	item_t *item;
	char *copy;
	char *s;

	assert(widget != NULL);
	assert(text != NULL);

	item = malloc(sizeof(item_t));
	item->widget = widget;
	item->text = list_new();

	copy = strdup(text);

	s = strtok(copy, "\n");

	while(s != NULL) {
		list_add(item->text, strdup(s));
		s = strtok(NULL, "\n");
	}

	free(copy);

	return item;
}

static void item_destroy(item_t *item)
{
	assert(item != NULL);

	list_destroy_item(item->text, free);
	free(item);
}

widget_t *statusbar_new(int x, int y)
{
	widget_statusbar_t *new;

	new = malloc(sizeof(widget_statusbar_t));
	new->selectElement = WIDGET_STATUSBAR_NONE_SELECT_ELEMENT;
	new->listElement = list_new();

	return widget_new(WIDGET_TYPE_STATUSBAR, x, y, WIDGET_STATUSBAR_WIDTH, WIDGET_STATUSBAR_HEIGHT, new);
}

void statusbar_add(widget_t *widget, widget_t *widget_catch, char *text)
{
	widget_statusbar_t *p;

	assert(widget != NULL);
	assert(widget_catch != NULL);
	assert(text != NULL);
	assert(widget->type == WIDGET_TYPE_STATUSBAR);

	p = (widget_statusbar_t *) widget->private_data;

	list_add(p->listElement, item_new(widget_catch, text));
}

void statusbar_draw(widget_t *widget)
{
	static image_t *g_image = NULL;
	widget_statusbar_t *p;

	assert(widget != NULL);
	assert(widget->type == WIDGET_TYPE_STATUSBAR);

	p = (widget_statusbar_t *) widget->private_data;

	if (g_image == NULL) {
		g_image = image_add("frame.png", IMAGE_ALPHA, "frame", IMAGE_GROUP_BASE);
	}

	image_draw(g_image, widget->x, widget->y, 0, 0, g_image->w, g_image->h);

	if (p->selectElement != WIDGET_STATUSBAR_NONE_SELECT_ELEMENT) {
		item_t *item;
		int i;

		item = (item_t *) p->listElement->list[p->selectElement];

		for (i = 0 ; i < item->text->count; i++) {
			char *s;
	
			s = (char *) item->text->list[i];
			font_draw(s, widget->x, widget->y + i * (font_get_size() + 5), COLOR_WHITE);
		}
	}
}

void statusbar_event(widget_t *widget)
{
	widget_statusbar_t *p;
	int i;

	assert(widget != NULL);
	assert(widget->type == WIDGET_TYPE_STATUSBAR);

	p = (widget_statusbar_t *) widget->private_data;

	p->selectElement = WIDGET_STATUSBAR_NONE_SELECT_ELEMENT;

	for (i = 0 ; i < p->listElement->count; i++) {
		item_t *item;

		item = (item_t *) p->listElement->list[i];

		if (mouse_buffer_is_on_area(item->widget->x, item->widget->y,
					    item->widget->w, item->widget->h,
					    MOUSE_BUF_MOTION)) {
			p->selectElement = i;
		}
	}
}

void statusbar_destroy(widget_t *widget)
{
	widget_statusbar_t *p;

	assert(widget != NULL);
	assert(widget->type == WIDGET_TYPE_STATUSBAR);

	p = (widget_statusbar_t *) widget->private_data;

	list_destroy_item(p->listElement, item_destroy);
	free(p);

	widget_destroy(widget);
}
