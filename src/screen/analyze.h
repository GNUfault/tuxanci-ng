#ifndef SCREEN_ANALYZE_H
#define SCREEN_ANALYZE_H

#include "main.h"

typedef struct analyze_struct {
	char *name;
	int score;
} analyze_t;

extern void analyze_start();
extern void analyze_draw();
extern void analyze_event();
extern void analyze_stop();
extern void analyze_restart();
extern void analyze_add(char *name, int score);
extern void analyze_set_msg(char *msg);
extern void analyze_end();
extern void analyze_init();
extern void analyze_quit();

#endif /* SCREEN_ANALYZE_H */
