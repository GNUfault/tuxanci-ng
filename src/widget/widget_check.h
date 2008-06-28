
#ifndef WIDGET_CHECK_H

#define WIDGET_CHECK_H

#include "base/main.h"

#define WIDGET_CHECK_TIME_SWITCH_STATUS	5

#define WIDGET_CHECK_WIDTH	25
#define WIDGET_CHECK_HEIGHT	25

typedef struct widget_check
{
	int x, y;
	bool_t status;
	int time;
	void (*fce_event)(void *);
} widget_check_t;

extern widget_check_t* newWidgetCheck(int x, int y, bool_t status, void (*fce_event)(void *));
extern void drawWidgetCheck(widget_check_t *p);
extern void eventWidgetCheck(widget_check_t *p);
extern void destroyWidgetCheck(widget_check_t *p);

#endif

