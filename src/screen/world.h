
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

extern bool_t word_is_inicialized();
extern void word_init();
extern void word_set_arena(arenaFile_t * arenaFile);
extern void word_do_end();
extern void word_inc_round();
extern tux_t *word_get_control_tux(int control_type);
extern void word_set_control_tux(tux_t * tux, int control_type);
extern void word_tux_control(tux_t * p);
extern void word_draw();
extern void word_event();
extern void word_start();
extern void word_stop();
extern void word_quit();

#endif
