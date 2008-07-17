
#ifndef WIDGET_IMAGE_H

#define WIDGET_IMAGE_H

#include "main.h"
#include "interface.h"

typedef struct widget_image
{
	int x, y;
	image_t *image;
} widget_image_t;

extern widget_image_t* newWidgetImage(int x, int y, image_t *image);
extern void drawWidgetImage(widget_image_t *p);
extern void eventWidgetImage(widget_image_t *p);
extern void destroyWidgetImage(widget_image_t *p);

#endif

