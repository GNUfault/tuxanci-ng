
#include <stdlib.h>
#include <assert.h>

#include "main.h"

#include "widget.h"
#include "widget_label.h"

widget_t* newWidget(int type, int x, int y, int w, int h, void *private_data)
{
	widget_t *new;

	new = malloc( sizeof(widget_t) );
	new->type = type;
	new->x = x;
	new->y = y;
	new->w = w;
	new->h = h;
	new->private_data = private_data;

	return new;
}

void widgetSetLocation(widget_t *p, int x, int y)
{
	p->x = x;
	p->y = y;
}

void widgetGetLocation(widget_t *p, int *x, int *y)
{
	if( x != NULL )*x = p->x;
	if( y != NULL )*y = p->y;
}

void widgetSetSize(widget_t *p, int w, int h)
{
	p->w = w;
	p->h = h;
}

void widgetGetSize(widget_t *p, int *w, int *h)
{
	if( w != NULL )*w = p->w;
	if( h != NULL )*h = p->h;
}

void destroyWidget(widget_t *p)
{
	free(p);
}
