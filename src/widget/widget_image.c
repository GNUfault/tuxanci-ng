
#include <stdlib.h>

#include <stdlib.h>
#include "main.h"
#include "image.h"
#include "interface.h"
#include "font.h"
#include "widget_image.h"

widget_image_t* newWidgetImage(int x, int y, image_t *image)
{
	widget_image_t *new;

	new = malloc( sizeof(widget_image_t) );
	new->x = x;
	new->y = y;
	new->image = image;

	return new;
}

void drawWidgetImage(widget_image_t *p)
{
	drawImage(p->image, p->x, p->y, 0, 0, p->image->w, p->image->h);
}

void eventWidgetImage(widget_image_t *p)
{
}

void destroyWidgetImage(widget_image_t *p)
{
	free(p);
}
