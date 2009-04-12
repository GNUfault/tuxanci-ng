#ifndef WIDGET_CATCHKEY_H

#    define WIDGET_CATCHKEY_H

#    include "main.h"
#    include "widget.h"

#    define WIDGET_CATCHKEY_WIDTH	110
#    define WIDGET_CATCHKEY_HEIGHT	12

#    define WIDGET_CATCHKEY_NOKEY	-1

typedef struct widget_catchkey_struct {
	int w, h;
	int key;
	bool_t active;
	void (*fce_event) (void *p);
} widget_catchkey_t;

extern widget_t *catchKey_new(int key, int x, int y, void *event);
extern int catchKey_get(widget_t * p);
extern void catchKey_set(widget_t * p, int key);
extern void catchKey_draw(widget_t * p);
extern void catchKey_event(widget_t * p);
extern void catchKey_destroy(widget_t * p);

#endif
