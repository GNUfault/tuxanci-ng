
#ifndef FONT_H

#define FONT_H

#include <SDL_ttf.h>
#include <assert.h>
#include "base/main.h"

#define COLOR_WHITE	255, 255, 255
#define COLOR_BLACK	0, 0, 0

#define COLOR_RED	255, 0, 0
#define COLOR_GREEN	0, 255, 0
#define COLOR_BLUE	0, 0, 255

#define COLOR_YELLOW	255, 255, 0

extern bool_t isFontInicialized();
extern void initFont(char *file,int size);
extern void drawFont(char *s,int x,int y,int r,int g,int b);
extern int getFontSize();
extern void getTextSize(char *s, int *w, int *h);
extern void quitFont();

#endif
