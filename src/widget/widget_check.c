
#include <stdlib.h>
#include <assert.h>

#include "main.h"
#include "interface.h"
#include "image.h"

#include "widget.h"
#include "widget_check.h"

widget_t *
newWidgetCheck(int x, int y, bool_t status, void (*fce_event) (void *))
{
    widget_check_t *new;

    new = malloc(sizeof(widget_check_t));
    new->time = 0;
    new->status = status;
    new->fce_event = fce_event;

    return newWidget(WIDGET_TYPE_CHECK, x, y, WIDGET_CHECK_WIDTH,
                     WIDGET_CHECK_HEIGHT, new);
}

void
drawWidgetCheck(widget_t * widget)
{
    widget_check_t *p;
    static image_t *g_check = NULL;
    int x, y;
    int offset;

    assert(widget != NULL);
    assert(widget->type == WIDGET_TYPE_CHECK);

    p = (widget_check_t *) widget->private_data;

    getMousePosition(&x, &y);

    if (g_check == NULL) {
        g_check =
            addImageData("check.png", IMAGE_ALPHA, "check", IMAGE_GROUP_BASE);
    }

    if (p->status == TRUE) {
        offset = 0;
    }
    else {
        offset = WIDGET_CHECK_WIDTH;
    }

    drawImage(g_check, widget->x, widget->y, offset, 0, WIDGET_CHECK_WIDTH,
              WIDGET_CHECK_HEIGHT);
}

void
eventWidgetCheck(widget_t * widget)
{
    widget_check_t *p;
    int x, y;

    assert(widget != NULL);
    assert(widget->type == WIDGET_TYPE_CHECK);

    p = (widget_check_t *) widget->private_data;

    if (p->time > 0) {
        p->time--;
        return;
    }

    getMousePosition(&x, &y);

    if (x >= widget->x && x <= widget->x + WIDGET_CHECK_WIDTH &&
        y >= widget->y && y <= widget->y + WIDGET_CHECK_HEIGHT &&
        isMouseClicked()) {
        if (p->status == TRUE) {
            p->status = FALSE;
            p->time = WIDGET_CHECK_TIME_SWITCH_STATUS;
        }
        else {
            p->status = TRUE;
            p->time = WIDGET_CHECK_TIME_SWITCH_STATUS;
        }

        if (p->fce_event != NULL) {
            p->fce_event(p);
        }
    }
}

bool_t
getWidgetCheckStatus(widget_t * widget)
{
    widget_check_t *p;

    assert(widget != NULL);
    assert(widget->type == WIDGET_TYPE_CHECK);

    p = (widget_check_t *) widget->private_data;

    return p->status;
}

void
setWidgetCheckStatus(widget_t * widget, bool_t status)
{
    widget_check_t *p;

    assert(widget != NULL);
    assert(widget->type == WIDGET_TYPE_CHECK);

    p = (widget_check_t *) widget->private_data;
    p->status = status;
}

void
destroyWidgetCheck(widget_t * widget)
{
    widget_check_t *p;

    assert(widget != NULL);
    assert(widget->type == WIDGET_TYPE_CHECK);

    p = (widget_check_t *) widget->private_data;
    free(p);
    destroyWidget(widget);
}
