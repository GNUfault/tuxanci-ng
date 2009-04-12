
#ifndef PAUZA_H

#    define PAUZA_H

#    define PAUZE_ACTIVE_TIME_INTERVAL	1000

#    include "main.h"

extern void pauza_init();
extern void pauza_draw();
extern void pauza_event();
extern bool_t pauza_is_active();
extern void pauza_quit();

#endif
