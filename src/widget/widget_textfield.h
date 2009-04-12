
#ifndef WIDGET_TEXTFIELD_H

#    define WIDGET_TEXTFIELD_H

#    include "main.h"
#    include "widget.h"

#    define WIDGET_TEXTFIELD_WIDTH		175
#    define WIDGET_TEXTFIELD_HEIGHT		36

#    define WIDGET_TEXTFIELD_TEXT_OFFSET_X	10

#    define WIDGET_TEXTFIELD_TIME_MULTIPLE 		10
#    define WIDGET_TEXTFIELD_TIME_READ_KEY 		2
#    define WIDGET_TEXTFIELD_TIME_BLICK_CURSOR	20

#    define WIDGET_TEXTFIELD_FILTER_ALL		0
#    define WIDGET_TEXTFIELD_FILTER_NUM		2
#    define WIDGET_TEXTFIELD_FILTER_ALPHANUM	3
#    define WIDGET_TEXTFIELD_FILTER_IP_OR_DOMAIN	4

typedef struct widget_textfield {
	int w, h;
	int canWrite;
	int timeBlick;
	int time;
	int atime;
	int filter;
	char text[STR_SIZE];
	bool_t active;
} widget_textfield_t;

extern widget_t *textField_new(char *text, int filter, int x, int y);
extern void textField_set_text(widget_t * widget, char *text);
extern char *textField_get_text(widget_t * widget);
extern void textField_draw(widget_t * widget);
extern void textField_event(widget_t * widget);
extern void textField_destroy(widget_t * widget);

#endif
