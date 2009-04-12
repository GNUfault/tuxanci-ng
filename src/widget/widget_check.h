
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

extern widget_t *check_new(int x, int y, bool_t status,
								void (*fce_event) (void *));
extern void check_draw(widget_t * widget);
extern void check_event(widget_t * widget);
extern bool_t check_get_status(widget_t * widget);
extern void check_set_status(widget_t * widget, bool_t status);
extern void check_destroy(widget_t * widget);

#endif
