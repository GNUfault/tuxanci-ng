
#ifndef WIDGET_CHOICEGROUP_H

#define WIDGET_CHOICEGROUP_H

#include "main.h"
#include "list.h"
#include "widget.h"

#define WIDGET_CHOICEGROUP_TIME_SWITCH_STATUS	5

#define WIDGET_CHOICEGROUP_WIDTH	25
#define WIDGET_CHOICEGROUP_HEIGHT	25

typedef struct widget_choicegroup
{
    bool_t status;
    int time;
    void (*fce_event) (void *);
    list_t *list;
} widget_choicegroup_t;

extern widget_t *newWidgetChoicegroup(int x, int y, bool_t status,
                                      list_t * list,
                                      void (*fce_event) (void *));

extern void drawWidgetChoicegroup(widget_t * p);
extern void setWidgetChoiceActive(widget_t * widget);
extern bool_t getWidgetChoiceStatus(widget_t * widget);
extern void setWidgetChoiceStatus(widget_t * widget, bool_t status);
extern void eventWidgetChoicegroup(widget_t * p);
extern void destroyWidgetChoicegroup(widget_t * p);

#endif
