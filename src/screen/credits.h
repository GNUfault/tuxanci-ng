#ifndef SCREEN_CREDITS_H
#define SCREEN_CREDITS_H

#define SCREEN_CREDITS_FILE		"AUTHORS"

#define SCREEN_CREDITS_OFFSET_MIN	130
#define SCREEN_CREDITS_OFFSET_MAX	WINDOW_SIZE_Y-120
#define SCREEN_CREDITS_OFFSET_RESTART	-1000
#define SCREEN_CREDITS_OFFSET_SPEED	2

extern void scredits_start();
extern void scredits_draw();
extern void scredits_event();
extern void scredits_stop();
extern void scredits_init();
extern void scredits_quit();

#endif /* SCREEN_CREDITS_H */
