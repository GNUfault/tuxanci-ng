#ifndef WIDGET_CATCHKEY_H
#define WIDGET_CATCHKEY_H

#include "main.h"
#include "widget.h"

#define WIDGET_CATCHKEY_NOKEY	-1

#define WIDGET_CATCHKEY_WIDTH	110
#define WIDGET_CATCHKEY_HEIGHT	16

typedef struct widget_catchkey_struct {
	int w, h;
	int key;
	bool_t active;
	void (*fce_event) (void *p);
} widget_catchkey_t;

extern widget_t *catch_key_new(int key, int x, int y, void *event);
extern int catch_key_get(widget_t *p);
extern void catch_key_set(widget_t *p, int key);
extern void catch_key_draw(widget_t *p);
extern void catch_key_event(widget_t *p);
extern void catch_key_destroy(widget_t *p);

#endif /* WIDGET_CATCHKEY_H */
