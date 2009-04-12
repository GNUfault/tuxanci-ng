
#ifndef WIDGET_IMAGE_H

#    define WIDGET_IMAGE_H

#    include "main.h"
#    include "interface.h"
#    include "widget.h"

typedef struct widget_image {
	image_t *image;
} widget_image_t;

extern widget_t *wid_image_new(int x, int y, image_t * image);
extern void wid_image_draw(widget_t * p);
extern void wid_image_event(widget_t * p);
extern void wid_image_destroy(widget_t * p);

#endif
