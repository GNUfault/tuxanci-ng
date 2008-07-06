
#ifndef SCREEN_CREDITS_H

#define SCREEN_CREDITS_H

#define SCREEN_CREDITS_FILE		"AUTHORS"

#define SCREEN_CREDITS_OFFSET_MIN	130
#define SCREEN_CREDITS_OFFSET_MAX	WINDOW_SIZE_Y-120
#define SCREEN_CREDITS_OFFSET_RESTART	-1000
#define SCREEN_CREDITS_OFFSET_SPEED	2

extern void startScreenCredits();
extern void drawScreenCredits();
extern void eventScreenCredits();
extern void stopScreenCredits();
extern void initScreenCredits();
extern void quitScreenCredits();

#endif
