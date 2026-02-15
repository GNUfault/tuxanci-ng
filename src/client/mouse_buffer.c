#include <SDL2/SDL.h>

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "main.h"
#include "list.h"
#include "mouse_buffer.h"
#include "interface.h"

static list_t *list_event;

typedef struct mouse_event_struct {
	Uint8 button;
	int x;
	int y;
} mouse_event_t;

static mouse_event_t *mouse_event_new(int x, int y, Uint8 button)
{
	mouse_event_t *mouse_event;

	mouse_event = malloc(sizeof(mouse_event_t));
	mouse_event->x = x;
	mouse_event->y = y;
	mouse_event->button = button;

	return mouse_event;
}

static void mouse_event_destroy(mouse_event_t *mouse_event)
{
	assert(mouse_event != NULL);
	free(mouse_event);
}

int mouse_buffer_init()
{
	list_event = list_new();

	return 0;
}

int mouse_buffer_event(SDL_MouseButtonEvent *button)
{
	assert(button != NULL);
	assert(list_event != NULL);

	switch (button->button) {
		case SDL_BUTTON_LEFT:
		case SDL_BUTTON_RIGHT:
			/*printf("mouse button - pos(%d,%d)\n", button->x, button->y);*/

			{
				int lx = button->x, ly = button->y;
				interface_window_to_logical(button->x, button->y, &lx, &ly);
				list_add(list_event, mouse_event_new(lx, ly, button->button));
			}

			return 1;
			break;

		default:
			break;
	}

	return 0;
}

int mouse_buffer_clean()
{
	assert(list_event != NULL);

	list_destroy_item(list_event, mouse_event_destroy);
	list_event = list_new();

	return 0;
}

bool_t mouse_buffer_is_on_area(int x, int y, int w, int h, unsigned int flag)
{
	mouse_event_t *mouse_event;
	int i;

	assert(list_event != NULL);

	if (flag & MOUSE_BUF_MOTION) {
		int mouse_x, mouse_y;

		SDL_GetMouseState(&mouse_x, &mouse_y);
		interface_window_to_logical(mouse_x, mouse_y, &mouse_x, &mouse_y);

		if (mouse_x >= x && mouse_y >= y &&
			mouse_x < x+w && mouse_y < y+h) {
			return TRUE;
		}
	} else {
		for (i = 0; i < list_event->count; i++) {
			mouse_event = (mouse_event_t *) list_event->list[i];

			if ((mouse_event->x >= x && mouse_event->y >= y &&
			     mouse_event->x < x + w && mouse_event->y < y + h) ||
			    (flag & MOUSE_BUF_AREA_NONE)) {
				if (flag & MOUSE_BUF_CLICK_LEFT) {
					if (mouse_event->button == SDL_BUTTON_LEFT) {
						return TRUE;
					} else {
						return FALSE;
					}
				}

				if (flag & MOUSE_BUF_CLICK_RIGHT) {
					if (mouse_event->button == SDL_BUTTON_RIGHT) {
						return TRUE;
					} else {
						return FALSE;
					}
				}

				if (flag & MOUSE_BUF_CLICK) {
					if (mouse_event->button == SDL_BUTTON_LEFT ||
					    mouse_event->button == SDL_BUTTON_RIGHT) {
						return TRUE;
					} else {
						return FALSE;
					}
				}

				return TRUE;
			}
		}
	}

	return FALSE;
}

int mouse_buffer_quit()
{
	assert(list_event != NULL);
	list_destroy_item(list_event, mouse_event_destroy);

	return 0;
}
