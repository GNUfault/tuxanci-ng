
#ifndef PANEL_H

#    define PANEL_H

#    include "main.h"
#    include "list.h"

#    define PANEL_SCORE_LOCATION_X ( WINDOW_SIZE_X / 2 - w / 2 )
#    define PANEL_SCORE_LOCATION_Y ( WINDOW_SIZE_Y - 40 )

#    define PANEL_SHOT_WIDTH	4
#    define PANEL_SHOT_HEIGHT	8

#    define PANEL_LOCATION_X	0
#    define PANEL_LOCATION_Y	(WINDOW_SIZE_Y-g_panel->h)

#    define PANEL_SHOT_FILL		0
#    define PANEL_SHOT_EMPTY	1
#    define PANEL_SHOT_LINE		2

extern bool_t panel_is_inicialized();
extern void panel_init();
extern void panel_draw(tux_t * tux_right, tux_t * tux_left);
extern void panel_quit();

#endif
