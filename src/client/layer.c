#include <stdlib.h>
#include <assert.h>

#include "main.h"
#include "list.h"
#include "tux.h"

#include "interface.h"
#include "layer.h"
#include "image.h"

#include "world.h"

static list_t *listLayer;

static bool_t isLayerInit = FALSE;

bool_t layer_is_inicialized()
{
	return isLayerInit;
}

/*
 * Initialize global list of the layers
 */
void layer_init()
{
	listLayer = list_new();
	isLayerInit = TRUE;
}

/*
 * Add image to queue for rendering
 * *img - image to be rendered
 * x,y - coordinates where to draw the image
 * w,h - width and height of rendered part of image
 * px,py - coordinates of upper left corner of rendered part of image
 * layer - on which layer to draw
 *         [0] - under Tuxanek; [1] same as Tuxanek; [2] over Tuxanek
 */
void addLayer(image_t *img, int x, int y, int px, int py, int w, int h, int player)
{
	layer_t *new;
	layer_t *actual;
	int actual_value;
	int new_value;
	int i;

	assert(img != NULL);

	new = malloc(sizeof(layer_t));
	new->x = x;
	new->y = y;
	new->w = w;
	new->h = h;
	new->px = px;
	new->py = py;
	new->layer = player;
	new->image = img;

	new_value = player * WINDOW_SIZE_Y + y + h;

	/* here we are sure, that we want to insert it somewhere else than the begining */
	for (i = 0; i < listLayer->count; i++) {
		actual = list_get(listLayer, i);

		actual_value = actual->layer * WINDOW_SIZE_Y + actual->y + actual->h;

		if (actual_value > new_value) {
			list_ins(listLayer, i, new);
			return;
		}
	}

	list_add(listLayer, new);
}

/*
 * Draws all items onto the screen according to the layer and coordinate Y+H
 * After drawing frees the list
 */
void layer_draw_all()
{
	layer_t *this;
	int i;

	for (i = 0; i < listLayer->count; i++) {
		this = (layer_t *) listLayer->list[i];

		image_draw(this->image, this->x, this->y, this->px, this->py, this->w, this->h);
	}

	/*printf("listLayer->count = %d\n", listLayer->count);*/

	list_destroy_item(listLayer, free);
	listLayer = list_new();
}

void layer_draw_center(int x, int y)
{
	layer_t *this;
	int screen_x, screen_y;
	int i;
	/*int count = 0;*/

	arena_get_center_screen(&screen_x, &screen_y, x, y, WINDOW_SIZE_X, WINDOW_SIZE_Y);

	for (i = 0; i < listLayer->count; i++) {
		this = (layer_t *) listLayer->list[i];

		if (this->x + this->w < screen_x ||
		    this->x > screen_x + WINDOW_SIZE_X ||
		    this->y + this->h < screen_y ||
		    this->y > screen_y + WINDOW_SIZE_Y) {
			continue;
		}
		/*count++;*/

		image_draw(this->image, this->x - screen_x, this->y - screen_y,
					this->px, this->py, this->w, this->h);

		/*printf("%d %d\n", this->x - screen_x, this->y - screen_y);*/
	}

	/*printf("count = %d\n", count);*/

	list_destroy_item(listLayer, free);
	listLayer = list_new();
}

void layer_draw_slpit(int local_x, int local_y, int x, int y, int w, int h)
{
	layer_t *this;
	int i;
	/*int count = 0;*/

	for (i = 0; i < listLayer->count; i++) {
		this = (layer_t *) listLayer->list[i];

		if (this->x + this->w < x || this->x > x + w ||
		    this->y + this->h < y || this->y > y + h) {
			continue;
		}

		/*count++;*/

		if (local_x + (this->x - x) < local_x) {
			int z;

			z = local_x - (local_x + (this->x - x));

			this->x += z;
			this->px += z;
			this->w -= z;
		}

		if (local_x + (this->x - x) + this->w > local_x + w) {
			this->w -= (local_x + (this->x - x) + this->w) - (local_x + w);
		}

		if (local_y + (this->y - y) < local_y) {
			int z;

			z = local_y - (local_y + (this->y - y));

			this->y += z;
			this->py += z;
			this->h -= z;
		}

		if (local_y + (this->y - y) + this->h > local_y + h) {
			this->h -= (local_y + (this->y - y) + this->h) - (local_y + h);
		}

		image_draw(this->image, local_x + (this->x - x), local_y + (this->y - y),
				       this->px, this->py, this->w, this->h);

		/*printf("%d %d\n", this->x - screen_x, this->y - screen_y);*/
	}

	/*printf("count = %d\n", count);*/

	list_destroy_item(listLayer, free);
	listLayer = list_new();
}

void layer_flush()
{
	list_destroy_item(listLayer, free);
	listLayer = list_new();
}

/*
 * Delete list of layers from memory
 */
void layer_quit()
{
	list_destroy_item(listLayer, free);
	isLayerInit = FALSE;
}
