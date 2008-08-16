#ifndef WIDGET_CATCHKEY_H

#define WIDGET_CATCHKEY_H

#include "main.h"
#include "widget.h"

#define WIDGET_CATCHKEY_WIDTH	110
#define WIDGET_CATCHKEY_HEIGHT	12

#define WIDGET_CATCHKEY_NOKEY	-1

typedef struct widget_catchkey_struct
{
	int w, h;
	int key;
	bool_t active;
	void (*fce_event)(void *p);
} widget_catchkey_t;

extern widget_t* newWidgetCatchkey(int key, int x, int y, void *event);
extern int getWidgetCatchKey(widget_t *p);
extern void setWidgetCatchKey(widget_t *p, int key);
extern void drawWidgetCatchkey(widget_t *p);
extern void eventWidgetCatchkey(widget_t *p);
extern void destroyWidgetCatchkey(widget_t *p);

#endif
