
#ifndef WIDGET_TEXTFIELD_H

#define WIDGET_TEXTFIELD_H

#include "main.h"

#define WIDGET_TEXTFIELD_WIDTH		125
#define WIDGET_TEXTFIELD_HEIGHT		36

#define WIDGET_TEXTFIELD_TEXT_OFFSET_X	10

#define WIDGET_TEXTFIELD_TIME_MULTIPLE 		10
#define WIDGET_TEXTFIELD_TIME_READ_KEY 		2
#define WIDGET_TEXTFIELD_TIME_BLICK_CURSOR	20

#define WIDGET_TEXTFIELD_FILTER_ALL		0
#define WIDGET_TEXTFIELD_FILTER_NUM		2
#define WIDGET_TEXTFIELD_FILTER_ALPHANUM	3
#define WIDGET_TEXTFIELD_FILTER_IP_OR_DOMAIN	4

typedef struct widget_textfield
{
	int x, y;
	int w, h;
	int canWrite;
	int timeBlick;
	int time;
	int atime;
	int filter;
	char text[STR_SIZE];
	bool_t active;
} widget_textfield_t;

extern widget_textfield_t* newWidgetTextfield(char *text, int filter, int x, int y);
extern void setWidgetTextFiledText(widget_textfield_t *p, char *text);
extern void drawWidgetTextfield(widget_textfield_t *p);
extern void eventWidgetTextfield(widget_textfield_t *p);
extern void destroyWidgetTextfield(widget_textfield_t *p);

#endif

