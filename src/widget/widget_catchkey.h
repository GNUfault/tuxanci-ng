
#ifndef WIDGET_CATCHKEY_H

#define WIDGET_CATCHKEY_H

#include "main.h"

#define WIDGET_CATCHKEY_WIDTH	64
#define WIDGET_CATCHKEY_HEIGHT	32

#define WIDGET_CATCHKEY_NOKEY	-1

typedef struct widget_catchkey_struct
{
	int x, y;
	int w, h;
	int key;
	bool_t active;
} widget_catchkey_t;

extern widget_catchkey_t* newWidgetCatchkey(int x, int y, int key);
extern int getWidgetCatchKey(widget_catchkey_t *p);
extern void setWidgetCatchKey(widget_catchkey_t *p, int key);
extern void drawWidgetCatchkey(widget_catchkey_t *p);
extern void eventWidgetCatchkey(widget_catchkey_t *p);
extern void destroyWidgetCatchkey(widget_catchkey_t *p);

#endif

