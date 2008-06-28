
#ifndef PANEL_H

#define PANEL_H

#include "base/main.h"
#include "base/list.h"

#define PANEL_SCORE_LOCATION_X ( WINDOW_SIZE_X / 2 - w / 2 )
#define PANEL_SCORE_LOCATION_Y ( WINDOW_SIZE_Y - 40 )

#define PANEL_SHOT_WIDTH	4
#define PANEL_SHOT_HEIGHT	8

#define PANEL_LOCATION_X	0
#define PANEL_LOCATION_Y	(WINDOW_SIZE_Y-g_panel->h)

#define PANEL_SHOT_FILL		0
#define PANEL_SHOT_EMPTY	1
#define PANEL_SHOT_LINE		2

extern bool_t isPanelInicialized();
extern void initPanel();
extern void drawPanel(list_t *listTux);
extern void quitPanel();

#endif
