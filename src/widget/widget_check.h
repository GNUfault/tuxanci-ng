
#ifndef WIDGET_CHECK_H

#    define WIDGET_CHECK_H

#    include "main.h"
#    include "widget.h"

#    define WIDGET_CHECK_TIME_SWITCH_STATUS	5

#    define WIDGET_CHECK_WIDTH	25
#    define WIDGET_CHECK_HEIGHT	25

typedef struct widget_check {
	bool_t status;
	int time;
	void (*fce_event) (void *);
} widget_check_t;

extern widget_t *newWidgetCheck(int x, int y, bool_t status,
								void (*fce_event) (void *));
extern void drawWidgetCheck(widget_t * widget);
extern void eventWidgetCheck(widget_t * widget);
extern bool_t getWidgetCheckStatus(widget_t * widget);
extern void setWidgetCheckStatus(widget_t * widget, bool_t status);
extern void destroyWidgetCheck(widget_t * widget);

#endif
