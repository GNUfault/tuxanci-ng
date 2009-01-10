
#ifndef WIDGET_IMAGE_H

#    define WIDGET_IMAGE_H

#    include "main.h"
#    include "interface.h"
#    include "widget.h"

typedef struct widget_image {
	image_t *image;
} widget_image_t;

extern widget_t *newWidgetImage(int x, int y, image_t * image);
extern void drawWidgetImage(widget_t * p);
extern void eventWidgetImage(widget_t * p);
extern void destroyWidgetImage(widget_t * p);

#endif
