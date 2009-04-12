
#ifndef WIDGET_BUTTON_H

#    define WIDGET_BUTTON_H

#    include "main.h"
#    include "widget.h"

#    define WIDGET_BUTTON_TIME	10

#    define WIDGET_BUTTON_WIDTH	125
#    define WIDGET_BUTTON_HEIGHT	36

typedef struct widget_button {
	char *text;
	int w, h;
	void (*fce_event) (void *);
} widget_button_t;

extern widget_t *button_new(char *text, int x, int y,
								 void (*fce_event) (void *));
extern void button_draw(widget_t * widget);
extern void button_event(widget_t * widget);
extern void button_destroy(widget_t * widget);

#endif
