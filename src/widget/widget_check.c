#include <stdlib.h>
#include <assert.h>

#include "main.h"
#include "interface.h"
#include "image.h"

#include "widget.h"
#include "widget_check.h"

widget_t *check_new(int x, int y, bool_t status,  void (*fce_event) (void *))
{
	widget_check_t *new;

	new = malloc(sizeof(widget_check_t));
	new->time = 0;
	new->status = status;
	new->fce_event = fce_event;

	return widget_new(WIDGET_TYPE_CHECK, x, y, WIDGET_CHECK_WIDTH, WIDGET_CHECK_HEIGHT, new);
}

void check_draw(widget_t *widget)
{
	widget_check_t *p;
	static image_t *g_check = NULL;
	int x, y;
	int offset;

	assert(widget != NULL);
	assert(widget->type == WIDGET_TYPE_CHECK);

	p = (widget_check_t *) widget->private_data;

	interface_get_mouse_position(&x, &y);

	if (g_check == NULL) {
		g_check = image_add("check.png", IMAGE_ALPHA, "check", IMAGE_GROUP_BASE);
	}

	if (p->status == TRUE) {
		offset = 0;
	} else {
		offset = WIDGET_CHECK_WIDTH;
	}

	image_draw(g_check, widget->x, widget->y, offset, 0, WIDGET_CHECK_WIDTH, WIDGET_CHECK_HEIGHT);
}

void check_event(widget_t *widget)
{
	widget_check_t *p;
	int x, y;

	assert(widget != NULL);
	assert(widget->type == WIDGET_TYPE_CHECK);

	p = (widget_check_t *) widget->private_data;

	if (p->time > 0) {
		p->time--;
		return;
	}

	interface_get_mouse_position(&x, &y);

	if (x >= widget->x && x <= widget->x + WIDGET_CHECK_WIDTH &&
	    y >= widget->y && y <= widget->y + WIDGET_CHECK_HEIGHT &&
	    interface_is_mouse_clicket()) {
		if (p->status == TRUE) {
			p->status = FALSE;
			p->time = WIDGET_CHECK_TIME_SWITCH_STATUS;
		} else {
			p->status = TRUE;
			p->time = WIDGET_CHECK_TIME_SWITCH_STATUS;
		}

		if (p->fce_event != NULL) {
			p->fce_event(widget);
		}
	}
}

bool_t check_get_status(widget_t *widget)
{
	widget_check_t *p;

	assert(widget != NULL);
	assert(widget->type == WIDGET_TYPE_CHECK);

	p = (widget_check_t *) widget->private_data;

	return p->status;
}

void check_set_status(widget_t *widget, bool_t status)
{
	widget_check_t *p;

	assert(widget != NULL);
	assert(widget->type == WIDGET_TYPE_CHECK);

	p = (widget_check_t *) widget->private_data;
	p->status = status;
}

void check_destroy(widget_t *widget)
{
	widget_check_t *p;

	assert(widget != NULL);
	assert(widget->type == WIDGET_TYPE_CHECK);

	p = (widget_check_t *) widget->private_data;
	free(p);
	widget_destroy(widget);
}
