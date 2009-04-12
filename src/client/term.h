
#ifndef TERM_H

#    define TERM_H

#    include "main.h"

#    define TERM_ACTIVE_TIME_INTERVAL		500
#    define TERM_REFRESH_TIME_INTERVAL		1000

extern void term_init();
extern void term_draw();
extern void term_event();
extern void term_quit();

#endif
