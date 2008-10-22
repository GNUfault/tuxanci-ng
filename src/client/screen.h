
#ifndef SCREEN_H

#define SCREEN_H

#include "main.h"

typedef struct screen_struct
{
    char *name;
    void (*fce_start) ();
    void (*fce_event) ();
    void (*fce_draw) ();
    void (*fce_stop) ();
} screen_t;

extern bool_t isScreenInicialized();

extern screen_t *newScreen(char *name,
                           void (*fce_start) (), void (*fce_event) (),
                           void (*fce_draw) (), void (*fce_stop) ());

extern void destroyScreen(screen_t * p);
extern void registerScreen(screen_t * p);
extern void initScreen();
extern void setScreen(char *name);
extern void switchScreen();
extern void startScreen(char *name);
extern char *getScreen();
extern void drawScreen();
extern void eventScreen();
extern void quitScreen();

#endif
