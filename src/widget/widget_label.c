
#include <stdlib.h>

#include "base/main.h"
#include "client/interface.h"
#include "client/font.h"
#include "widget_label.h"

widget_label_t* newWidgetLabel(char *text, int x, int y, int bind)
{
	widget_label_t *new;

	new = malloc( sizeof(widget_label_t) );
	new->text = strdup(text);
	new->x = x;
	new->y = y;
	new->bind = bind;
	getTextSize(text, &new->w, &new->h);

	return new;
}

void drawWidgetLabel(widget_label_t *p)
{
	switch( p->bind )
	{
		case WIDGET_LABEL_RIGHT :
			drawFont(p->text, p->x+p->w, p->y + p->h/2, COLOR_WHITE);
		break;
		case WIDGET_LABEL_LEFT :
			drawFont(p->text, p->x, p->y + p->h/2, COLOR_WHITE);
		break;
		case WIDGET_LABEL_CENTER :
			drawFont(p->text, p->x-p->w/2, p->y + p->h/2, COLOR_WHITE);
		break;
	}
}

void eventWidgetLabel(widget_label_t *p)
{
}

void destroyWidgetLabel(widget_label_t *p)
{
	free(p->text);
	free(p);
}
