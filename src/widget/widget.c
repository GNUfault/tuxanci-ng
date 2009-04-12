
#include <stdlib.h>
#include <assert.h>

#include "main.h"

#include "widget.h"
#include "widget_label.h"

widget_t *widget_new(int type, int x, int y, int w, int h, void *private_data)
{
	widget_t *new;

	new = malloc(sizeof(widget_t));
	new->type = type;
	new->x = x;
	new->y = y;
	new->w = w;
	new->h = h;
	new->private_data = private_data;

	return new;
}

void widget_set_location(widget_t * p, int x, int y)
{
	p->x = x;
	p->y = y;
}

void widget_get_location(widget_t * p, int *x, int *y)
{
	if (x != NULL)
		*x = p->x;

	if (y != NULL)
		*y = p->y;
}

void widget_get_size(widget_t * p, int w, int h)
{
	p->w = w;
	p->h = h;
}

void widget_set_size(widget_t * p, int *w, int *h)
{
	if (w != NULL)
		*w = p->w;

	if (h != NULL)
		*h = p->h;
}

void widget_destroy(widget_t * p)
{
	free(p);
}
