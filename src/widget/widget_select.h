
#ifndef WIDGET_SELECT_H

#define WIDGET_SELECT_H

#include "main.h"
#include "list.h"
#include "widget.h"

typedef struct widget_select
{
    int w, h;
    list_t *list;
    int select;
    void (*fce_event) (void *);
} widget_select_t;

extern widget_t *newWidgetSelect(int x, int y, void (*fce_event) (void *));
extern char *getWidgetSelectItem(widget_t * widget);
extern int getWidgetSelectIndex(widget_t * widget);
extern void addToWidgetSelect(widget_t * widget, char *s);
extern void removeAllFromWidgetSelect(widget_t * widget);
extern void drawWidgetSelect(widget_t * widget);
extern void eventWidgetSelect(widget_t * widget);
extern void destroyWidgetSelect(widget_t * widget);

#endif
