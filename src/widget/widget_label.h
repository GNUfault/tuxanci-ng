
#ifndef WIDGET_LABEL_H

#define WIDGET_LABEL_H

#include "main.h"
#include "widget.h"

typedef struct widget_label
{
    int w, h;
    char *text;
    int bind;
} widget_label_t;

#define WIDGET_LABEL_RIGHT	1
#define WIDGET_LABEL_LEFT	2
#define WIDGET_LABEL_CENTER	3

extern widget_t *newWidgetLabel(char *text, int x, int y, int bind);
extern void drawWidgetLabel(widget_t * widget);
extern void eventWidgetLabel(widget_t * widget);
extern void destroyWidgetLabel(widget_t * widget);

#endif
