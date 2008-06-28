
#include <stdlib.h>
#include "base/main.h"
#include "client/interface.h"
#include "client/font.h"
#include "widget_select.h"

/*
typedef struct widget_select
{
	int x, y;
	int w, h;
	list_t *list;
	void (*fce_event)(void *);
} widget_select_t;
*/

widget_select_t* newWidgetSelect(int x, int y, void (*fce_event)(void *))
{
	widget_select_t *new;

	new = malloc( sizeof(widget_select_t) );
	new->x = x;
	new->y = y;
	new->select = 0;
	new->fce_event = fce_event;
	new->list = newList();

	return new;
}

void addToWidgetSelect(widget_select_t *p, char *s)
{
	addList(p->list, strdup(s) );
}

void drawWidgetSelect(widget_select_t *p)
{
	int x, y, w, h;
	int i;

	getMousePosition(&x, &y);

	for( i = 0 ; i < p->list->count ; i++ )
	{
		char *line;

		line = (char *)p->list->list[i];

		getTextSize(line, &w, &h);

		if( x > p->x && y > p->y + i*20 && x < p->x+w && y < p->y + i*20 + h )
		{
			drawFont(line, p->x, p->y + i*20, COLOR_YELLOW);
		}
		else
		{
			if( p->select == i )
			{
				drawFont(line, p->x, p->y + i*20, COLOR_RED);
			}
			else
			{
				drawFont(line, p->x, p->y + i*20, COLOR_WHITE);
			}
		}
	}
}

void eventWidgetSelect(widget_select_t *p)
{
	int x, y, w, h;
	int i;

	getMousePosition(&x, &y);

	for( i = 0 ; i < p->list->count ; i++ )
	{
		char *line;

		line = (char *)p->list->list[i];

		getTextSize(line, &w, &h);

		if( x > p->x && y > p->y + i*20 && x < p->x+w && y < p->y + i*20 + h && isMouseClicked() )
		{
			p->select = i;
			p->fce_event(p);
		}
	}
}

void destroyWidgetSelect(widget_select_t *p)
{
	destroyListItem(p->list, free);
	free(p);
}
