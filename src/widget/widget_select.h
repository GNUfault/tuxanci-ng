
#ifndef WIDGET_SELECT_H

#    define WIDGET_SELECT_H

#    include "main.h"
#    include "list.h"
#    include "widget.h"

typedef struct widget_select {
	int w, h;
	list_t *list;
	int select;
	void (*fce_event) (void *);
} widget_select_t;

extern widget_t *select_new(int x, int y, void (*fce_event) (void *));
extern char *select_get_item(widget_t * widget);
extern int select_get_index(widget_t * widget);
extern void select_add(widget_t * widget, char *s);
extern void select_remove_all(widget_t * widget);
extern void select_draw(widget_t * widget);
extern void select_event(widget_t * widget);
extern void select_destroy(widget_t * widget);

#endif
