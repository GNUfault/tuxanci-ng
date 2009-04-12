
#ifndef SCREEN_H

#    define SCREEN_H

#    include "main.h"

typedef struct screen_struct {
	char *name;
	void (*fce_start) ();
	void (*fce_event) ();
	void (*fce_draw) ();
	void (*fce_stop) ();
} screen_t;

extern bool_t screen_is_inicialized();

extern screen_t *screen_new(char *name,
						   void (*fce_start) (), void (*fce_event) (),
						   void (*fce_draw) (), void (*fce_stop) ());

extern void screen_destroy(screen_t * p);
extern void screen_register(screen_t * p);
extern void screen_init();
extern void screen_set(char *name);
extern void screen_switch();
extern void screen_start(char *name);
extern char *screen_get();
extern void screen_draw();
extern void screen_event();
extern void screen_quit();

#endif
