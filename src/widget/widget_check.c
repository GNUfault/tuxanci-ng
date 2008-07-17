
#include <stdlib.h>
#include "main.h"
#include "interface.h"
#include "image.h"
#include "widget_check.h"

widget_check_t* newWidgetCheck(int x, int y, bool_t status, void (*fce_event)(void *))
{
	widget_check_t *new;

	new = malloc( sizeof(widget_check_t) );
	new->x = x;
	new->y = y;
	new->time = 0;
	new->status = status;
	new->fce_event = fce_event;

	return new;
}

void drawWidgetCheck(widget_check_t *p)
{
	static image_t *g_check = NULL;
	int x, y;
	int offset;

	getMousePosition(&x, &y);

	if( g_check == NULL )
	{
		g_check = addImageData("check.png", IMAGE_ALPHA, "check", IMAGE_GROUP_BASE);
	}

	if( p->status == TRUE )
	{
		offset = 0;
	}
	else
	{
		offset = WIDGET_CHECK_WIDTH;
	}

	drawImage(g_check, p->x, p->y, offset, 0, WIDGET_CHECK_WIDTH, WIDGET_CHECK_HEIGHT);
}

void eventWidgetCheck(widget_check_t *p)
{
	int x, y;

	if( p->time > 0 )
	{
		p->time--;
		return;
	}

	getMousePosition(&x, &y);

	if( x >= p->x && x <= p->x+WIDGET_CHECK_WIDTH && 
	    y >= p->y && y <= p->y+WIDGET_CHECK_HEIGHT &&
	    isMouseClicked() )
	{
		if( p->status == TRUE )
		{
			p->status = FALSE;
			p->time = WIDGET_CHECK_TIME_SWITCH_STATUS;
		}
		else
		{
			p->status = TRUE;
			p->time = WIDGET_CHECK_TIME_SWITCH_STATUS;
		}

		if( p->fce_event != NULL )
		{
			p->fce_event(p);
		}
	}
}

void destroyWidgetCheck(widget_check_t *p)
{
	free(p);
}
