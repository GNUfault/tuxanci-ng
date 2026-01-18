#ifndef MY_LAYER_H
#define MY_LAYER_H

#include "main.h"
#include "image.h"

typedef struct layer_str {
	/* coordinates of the image */
	int x;
	int y;

	/* size of the image */
	int w;
	int h;

	/* coordinates of a part of the surface */
	int px;
	int py;

	/* size of a part of the surface */
	int pw;
	int ph;

	/* in which layer to lay it */
	int layer;

	/* image of the surface */
	image_t *image;
} layer_t;

extern bool_t layer_is_inicialized();
extern void layer_init();

extern void addLayer(image_t *img, int x, int y, int px, int py, int w, int h, int player);

extern void layer_draw_all(int x, int y); /* Fixed by GNUfault */
extern void layer_draw_center(int x, int y);
extern void layer_draw_slpit(int local_x, int local_y, int x, int y, int w, int h);
extern void layer_flush();
extern void layer_quit();

#endif /* MY_LAYER_H */
