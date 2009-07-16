#ifndef MOUSE_EVENT_H
#define MOUSE_EVENT_H

#include <SDL.h>

#define MOUSE_BUF_MOTION	1
#define MOUSE_BUF_CLICK_LEFT	2
#define MOUSE_BUF_CLICK_RIGHT	4
#define MOUSE_BUF_CLICK		8
#define MOUSE_BUF_AREA_NONE	16

extern int mouse_buffer_init();
extern int mouse_buffer_event(SDL_MouseButtonEvent *button);
extern int mouse_buffer_clean();
extern bool_t mouse_buffer_is_on_area(int x, int y, int w, int h, unsigned int flag);
extern int mouse_buffer_quit();

#endif /* MOUSE_EVENT_H */
