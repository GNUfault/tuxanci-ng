#include <stdlib.h>
#include <assert.h>

#include "main.h"
#include "interface.h"
#include "image.h"

#include "widget.h"
#include "widget_choicegroup.h"

widget_t *choice_group_new(int x, int y, bool_t status, list_t *list, void (*fce_event) (void *))
{
	widget_choicegroup_t *new;
	widget_t *widget;

	new = malloc(sizeof(widget_choicegroup_t));
	new->time = 0;
	new->status = status;
	new->fce_event = fce_event;
	new->list = list;

	widget = widget_new(WIDGET_TYPE_CHOICE, x, y, WIDGET_CHOICEGROUP_WIDTH, WIDGET_CHOICEGROUP_HEIGHT, new);

	list_add(new->list, widget);

	return widget;
}

void choice_group_draw(widget_t *widget)
{
	widget_choicegroup_t *p;
	static image_t *g_choicegroup = NULL;
	int x, y;
	int offset;

	assert(widget != NULL);
	assert(widget->type == WIDGET_TYPE_CHOICE);

	p = (widget_choicegroup_t *) widget->private_data;

	interface_get_mouse_position(&x, &y);

	if (g_choicegroup == NULL) {
		g_choicegroup = image_add("choice.png", IMAGE_ALPHA, "choicegroup", IMAGE_GROUP_BASE);
	}

	if (p->status == TRUE) {
		offset = 0;
	} else {
		offset = WIDGET_CHOICEGROUP_WIDTH;
	}

	image_draw(g_choicegroup, widget->x, widget->y, offset, 0,
		   WIDGET_CHOICEGROUP_WIDTH, WIDGET_CHOICEGROUP_HEIGHT);
}

void choice_group_set_active(widget_t *widget)
{
	widget_choicegroup_t *p;
	int i;

	assert(widget != NULL);
	assert(widget->type == WIDGET_TYPE_CHOICE);

	p = (widget_choicegroup_t *) widget->private_data;

	for (i = 0; i < p->list->count; i++) {
		widget_t *this;
		widget_choicegroup_t *thisChoice;

		this = (widget_t *) p->list->list[i];
		thisChoice = (widget_choicegroup_t *) this->private_data;

		if (widget == this) {
			thisChoice->status = TRUE;
			thisChoice->time = WIDGET_CHOICEGROUP_TIME_SWITCH_STATUS;
			thisChoice->fce_event(p);
		} else {
			thisChoice->status = FALSE;
		}
	}
}

bool_t choice_group_get_status(widget_t *widget)
{
	widget_choicegroup_t *p;

	assert(widget != NULL);
	assert(widget->type == WIDGET_TYPE_CHOICE);

	p = (widget_choicegroup_t *) widget->private_data;

	return p->status;
}

void choiceGroup_set_status(widget_t *widget, bool_t status)
{
	widget_choicegroup_t *p;

	assert(widget != NULL);
	assert(widget->type == WIDGET_TYPE_CHOICE);

	p = (widget_choicegroup_t *) widget->private_data;
	p->status = status;
}

void choice_group_event(widget_t *widget)
{
	widget_choicegroup_t *p;
	int x, y;

	assert(widget != NULL);
	assert(widget->type == WIDGET_TYPE_CHOICE);

	p = (widget_choicegroup_t *) widget->private_data;

	if (p->time > 0) {
		p->time--;
		return;
	}

	interface_get_mouse_position(&x, &y);

	if (x >= widget->x && x <= widget->x + WIDGET_CHOICEGROUP_WIDTH &&
	    y >= widget->y && y <= widget->y + WIDGET_CHOICEGROUP_HEIGHT &&
	    interface_is_mouse_clicket()) {
		choice_group_set_active(widget);
	}
}

void choice_group_destroy(widget_t *widget)
{
	widget_choicegroup_t *p;
	int my_index;

	assert(widget != NULL);
	assert(widget->type == WIDGET_TYPE_CHOICE);

	p = (widget_choicegroup_t *) widget->private_data;
	my_index = list_search(p->list, widget);
	assert(my_index != -1);
	list_del(p->list, my_index);

	free(p);
	widget_destroy(widget);
}
