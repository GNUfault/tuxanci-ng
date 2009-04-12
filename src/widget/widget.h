
#ifndef WIDGET_H

#    define WIDGET_H

#    include "main.h"

#    define WIDGET_TYPE_LABEL		1
#    define WIDGET_TYPE_BUTTON		2
#    define WIDGET_TYPE_BUTTONIMAGE		3
#    define WIDGET_TYPE_TEXTFILED		4
#    define WIDGET_TYPE_CATCHKEY		4
#    define WIDGET_TYPE_CHECK		5
#    define WIDGET_TYPE_CHOICE		6
#    define WIDGET_TYPE_IMAGE		7
#    define WIDGET_TYPE_SELECT		8

typedef struct widget_struct {
	int type;
	int x, y;
	int w, h;
	void *private_data;
} widget_t;

extern widget_t *widget_new(int type, int x, int y, int w, int h,
						   void *private_data);
extern void widget_set_location(widget_t * p, int x, int y);
extern void widget_get_location(widget_t * p, int *x, int *y);
extern void widget_get_size(widget_t * p, int w, int h);
extern void widget_set_size(widget_t * p, int *w, int *h);
extern void widget_destroy(widget_t * p);

#endif
