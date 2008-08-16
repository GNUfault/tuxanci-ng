
#ifndef WIDGET_H

#define WIDGET_H

#include "main.h"

#define WIDGET_TYPE_LABEL		1
#define WIDGET_TYPE_BUTTON		2
#define WIDGET_TYPE_BUTTONIMAGE		3
#define WIDGET_TYPE_TEXTFILED		4
#define WIDGET_TYPE_CATCHKEY		4
#define WIDGET_TYPE_CHECK		5
#define WIDGET_TYPE_CHOICE		6
#define WIDGET_TYPE_IMAGE		7
#define WIDGET_TYPE_SELECT		8

typedef struct widget_struct
{
	int type;
	int x, y;
	int w, h;
	void *private_data;	
} widget_t;

extern widget_t* newWidget(int type, int x, int y, int w, int h, void *private_data);
extern void widgetSetLocation(widget_t *p, int x, int y);
extern void widgetGetLocation(widget_t *p, int *x, int *y);
extern void widgetSetSize(widget_t *p, int w, int h);
extern void widgetGetSize(widget_t *p, int *w, int *h);
extern void destroyWidget(widget_t *p);

#endif
