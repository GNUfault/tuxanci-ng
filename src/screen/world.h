
#ifndef SCREEN_WORLD_H

#    define SCREEN_WORLD_H

#    include "main.h"
#    include "myTimer.h"
#    include "tux.h"
#    include "arenaFile.h"
#    include "arena.h"

#    define TIMER_END_ARENA 5000
#    define WORLD_COUNT_ROUND_UNLIMITED	-1
#    define LAG_SERVER_UNKNOWN		-1

extern bool_t world_is_inicialized();
extern void world_init();
extern void world_set_arena(arenaFile_t * arenaFile);
extern void world_do_end();
extern void world_inc_round();
extern tux_t *world_get_control_tux(int control_type);
extern void world_set_control_tux(tux_t * tux, int control_type);
extern void world_tux_control(tux_t * p);
extern void world_draw();
extern void world_event();
extern void world_start();
extern void world_stop();
extern void world_quit();

#endif
