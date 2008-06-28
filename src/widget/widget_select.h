
#ifndef WIDGET_SELECT_H

#define WIDGET_SELECT_H

#include "base/main.h"
#include "base/list.h"


typedef struct widget_select
{
	int x, y;
	int w, h;
	list_t *list;
	int select;
	void (*fce_event)(void *);
} widget_select_t;

extern widget_select_t* newWidgetSelect(int x, int y, void (*fce_event)(void *));
extern void addToWidgetSelect(widget_select_t *p, char *s);
extern void drawWidgetSelect(widget_select_t *p);
extern void eventWidgetSelect(widget_select_t *p);
extern void destroyWidgetSelect(widget_select_t *p);

#endif

