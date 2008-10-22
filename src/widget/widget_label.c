
#include <stdlib.h>
#include <assert.h>

#include "main.h"
#include "interface.h"
#include "font.h"
#include "widget_label.h"
#include "widget.h"

widget_t *
newWidgetLabel(char *text, int x, int y, int bind)
{
    widget_label_t *new;

    new = malloc(sizeof(widget_label_t));
    new->text = strdup(text);
    new->bind = bind;
    getTextSize(text, &(new->w), &(new->h));

    return newWidget(WIDGET_TYPE_LABEL, x, y, new->w, new->h, new);
}

void
drawWidgetLabel(widget_t * widget)
{
    widget_label_t *p;

    assert(widget != NULL);
    assert(widget->type == WIDGET_TYPE_LABEL);

    p = (widget_label_t *) widget->private_data;

    switch (p->bind) {
    case WIDGET_LABEL_RIGHT:
        drawFont(p->text, widget->x + p->w, widget->y + p->h / 2,
                 COLOR_WHITE);
        break;
    case WIDGET_LABEL_LEFT:
        drawFont(p->text, widget->x, widget->y + widget->h / 2, COLOR_WHITE);
        break;
    case WIDGET_LABEL_CENTER:
        drawFont(p->text, widget->x - p->w / 2, widget->y + p->h / 2,
                 COLOR_WHITE);
        break;
    }
}

void
eventWidgetLabel(widget_t * widget)
{
    assert(widget != NULL);
    assert(widget->type == WIDGET_TYPE_LABEL);
}

void
destroyWidgetLabel(widget_t * widget)
{
    widget_label_t *p;

    assert(widget != NULL);
    assert(widget->type == WIDGET_TYPE_LABEL);

    p = (widget_label_t *) widget->private_data;

    free(p->text);
    free(p);

    destroyWidget(widget);
}
