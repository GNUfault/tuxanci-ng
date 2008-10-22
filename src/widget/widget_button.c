
#include <stdlib.h>
#include <assert.h>

#include "main.h"

#include "interface.h"
#include "image.h"
#include "font.h"

#include "widget.h"
#include "widget_button.h"

widget_t *
newWidgetButton(char *text, int x, int y, void (*fce_event) (void *))
{
    widget_button_t *new;

    new = malloc(sizeof(widget_button_t));
    new->text = strdup(text);
    new->fce_event = fce_event;
    getTextSize(text, &(new->w), &(new->h));

    return newWidget(WIDGET_TYPE_BUTTON, x, y, WIDGET_BUTTON_WIDTH,
                     WIDGET_BUTTON_HEIGHT, new);
}

void
drawWidgetButton(widget_t * widget)
{
    widget_button_t *p;
    static image_t *g_button0 = NULL;
    static image_t *g_button1 = NULL;
    int x, y;

    assert(widget != NULL);
    assert(widget->type == WIDGET_TYPE_BUTTON);

    p = (widget_button_t *) widget->private_data;
    getMousePosition(&x, &y);

    if (g_button0 == NULL) {
        g_button0 =
            addImageData("button0.png", IMAGE_ALPHA, "button0",
                         IMAGE_GROUP_BASE);
    }

    if (g_button1 == NULL) {
        g_button1 =
            addImageData("button1.png", IMAGE_ALPHA, "button1",
                         IMAGE_GROUP_BASE);
    }

    if (x >= widget->x && x <= widget->x + WIDGET_BUTTON_WIDTH &&
        y >= widget->y && y <= widget->y + WIDGET_BUTTON_HEIGHT) {
        drawImage(g_button1, widget->x, widget->y, 0, 0, g_button0->w,
                  g_button0->h);
    }
    else {
        drawImage(g_button0, widget->x, widget->y, 0, 0, g_button0->w,
                  g_button0->h);
    }

    //drawFont(p->text, p->x+WIDGET_BUTTON_WIDTH/2-p->w/2, p->y+p->h/2, COLOR_WHITE);
    drawFont(p->text,
             widget->x + WIDGET_BUTTON_WIDTH / 2 - p->w / 2,
             widget->y + WIDGET_BUTTON_HEIGHT / 2 - p->h / 2, COLOR_WHITE);
}

void
eventWidgetButton(widget_t * widget)
{
    widget_button_t *p;
    static int time = 0;
    int x, y;

    assert(widget != NULL);
    assert(widget->type == WIDGET_TYPE_BUTTON);

    p = (widget_button_t *) widget->private_data;

    if (time > 0) {
        time--;
        return;
    }

    getMousePosition(&x, &y);

    if (x >= widget->x && x <= widget->x + WIDGET_BUTTON_WIDTH &&
        y >= widget->y && y <= widget->y + WIDGET_BUTTON_HEIGHT &&
        isMouseClicked()) {
        time = WIDGET_BUTTON_TIME;
#ifdef DEBUG
        printf(_("Event caught\n"));
#endif
        p->fce_event(widget);
    }
}

void
destroyWidgetButton(widget_t * widget)
{
    widget_button_t *p;

    assert(widget != NULL);
    assert(widget->type == WIDGET_TYPE_BUTTON);

    p = (widget_button_t *) widget->private_data;

    free(p->text);
    free(p);

    destroyWidget(widget);
}
