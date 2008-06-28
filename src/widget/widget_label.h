
#ifndef WIDGET_LABEL_H

#define WIDGET_LABEL_H

#include "base/main.h"

typedef struct widget_label
{
	int x, y;
	int w, h;
	char *text;
	int bind;
} widget_label_t;

#define WIDGET_LABEL_RIGHT	1
#define WIDGET_LABEL_LEFT	2
#define WIDGET_LABEL_CENTER	3

extern widget_label_t* newWidgetLabel(char *text, int x, int y, int bind);
extern void drawWidgetLabel(widget_label_t *p);
extern void eventWidgetLabel(widget_label_t *p);
extern void destroyWidgetLabel(widget_label_t *p);

#endif

