
#ifndef WIDGET_IMAGE_H

#define WIDGET_IMAGE_H

#include "base/main.h"
#include "client/interface.h"

typedef struct widget_image
{
	int x, y;
	SDL_Surface *image;
} widget_image_t;

extern widget_image_t* newWidgetImage(int x, int y, SDL_Surface *image);
extern void drawWidgetImage(widget_image_t *p);
extern void eventWidgetImage(widget_image_t *p);
extern void destroyWidgetImage(widget_image_t *p);

#endif

