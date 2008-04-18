
#ifndef WIDGET_TEXTFIELD_H

#define WIDGET_TEXTFIELD_H

#include "main.h"

#define WIDGET_TEXTFIELD_WIDTH		188
#define WIDGET_TEXTFIELD_HEIGHT		36

#define WIDGET_TEXTFIELD_TEXT_OFFSET_X	10

#define WIDGET_TEXTFIELD_TIME_READ_KEY 		2
#define WIDGET_TEXTFIELD_TIME_BLICK_CURSOR	20

typedef struct widget_textfield
{
	int x, y;
	int w, h;
	int canWrite;
	int timeBlick;
	int time;
	char text[STR_SIZE];
	bool_t active;
} widget_textfield_t;

extern widget_textfield_t* newWidgetTextfield(char *text, int x, int y);
extern void drawWidgetTextfield(widget_textfield_t *p);
extern void eventWidgetTextfield(widget_textfield_t *p);
extern void destroyWidgetTextfield(widget_textfield_t *p);

#endif

