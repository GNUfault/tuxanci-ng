
#ifndef WIDGET_BUTTONIMAGE_H

#include "main.h"

#define WIDGET_BUTTONIMAGE_H

#define WIDGET_BUTTONIMAGE_TIME	10

typedef struct widget_buttonimageimage
{
	int x, y;
	int w, h;
	int active;
	image_t *image;
	void (*fce_event)(void *);
} widget_buttonimage_t;

extern widget_buttonimage_t* newWidgetButtonimage(image_t *image, int x, int y,
		void (*fce_event)(void *));

extern void drawWidgetButtonimage(widget_buttonimage_t *p);
extern void eventWidgetButtonimage(widget_buttonimage_t *p);
extern void destroyWidgetButtonimage(widget_buttonimage_t *p);

#endif

