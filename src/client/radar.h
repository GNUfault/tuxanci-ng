
#ifndef RADAR_H

#    define RADAR_H

#    include "arena.h"

#    define RADAR_LOCATION_X (WINDOW_SIZE_X-110)
#    define RADAR_LOCATION_Y (10)

#    define RADAR_SIZE_X 100
#    define RADAR_SIZE_Y 100

#    define RADAR_TYPE_YOU	3
#    define RADAR_TYPE_TUX	1
#    define RADAR_TYPE_ITEM	2

extern void radar_init();
extern void radar_add(int id, int x, int y, int type);
extern void radar_del(int id);
extern void radar_draw(arena_t * arena);
extern void radar_quit();

#endif
