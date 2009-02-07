
#ifndef WIDGET_BUTTONIMAGE_H

#    include "main.h"

#    define WIDGET_BUTTONIMAGE_H

#    define WIDGET_BUTTONIMAGE_TIME	10

#    include "widget.h"

typedef struct widget_buttonimageimage {
	int w, h;
	int active;
	image_t *image;
	void (*fce_event) (void *);
} widget_buttonimage_t;

extern widget_t *newWidgetButtonimage(image_t * image, int x, int y,
									  void (*fce_event) (void *));

extern void setWidgetButtonimageActive(widget_t * widget, bool_t active);
extern void drawWidgetButtonimage(widget_t * widget);
extern void eventWidgetButtonimage(widget_t * widget);
extern void destroyWidgetButtonimage(widget_t * widget);

#endif
