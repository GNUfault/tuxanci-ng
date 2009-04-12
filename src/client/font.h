
#ifndef FONT_H

#    define FONT_H

#    include <SDL_ttf.h>
#    include <assert.h>
#    include "main.h"

#    define COLOR_WHITE	255, 255, 255
#    define COLOR_BLACK	0, 0, 0

#    define COLOR_RED	255, 0, 0
#    define COLOR_GREEN	0, 255, 0
#    define COLOR_BLUE	0, 0, 255

#    define COLOR_YELLOW	255, 255, 0
#    define FONT_SIZE	16

extern bool_t font_is_inicialized();
extern void font_init();
extern void font_draw(char *s, int x, int y, int r, int g, int b);
extern void font_drawMaxSize(char *s, int x, int y, int w, int h, int r, int g,
							int b);
extern int font_get_size();
extern void font_text_size(char *s, int *w, int *h);
extern void font_quit();

#endif
