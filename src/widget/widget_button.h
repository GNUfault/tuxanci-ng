
#ifndef WIDGET_BUTTON_H

#define WIDGET_BUTTON_H

#include "base/main.h"

#define WIDGET_BUTTON_TIME	10

#define WIDGET_BUTTON_WIDTH	125
#define WIDGET_BUTTON_HEIGHT	36

typedef struct widget_button
{
	int x, y;
	int w, h;
	char *text;
	void (*fce_event)(void *);
} widget_button_t;

extern widget_button_t* newWidgetButton(char *text, int x, int y, void (*fce_event)(void *));
extern void drawWidgetButton(widget_button_t *p);
extern void eventWidgetButton(widget_button_t *p);
extern void destroyWidgetButton(widget_button_t *p);

#endif

