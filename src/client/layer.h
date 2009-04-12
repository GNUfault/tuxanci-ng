
#ifndef MY_LAYER_H

#    define MY_LAYER_H

#    include "main.h"
#    include "image.h"

typedef struct layer_str {
	int x;						//suradnice obrazka na obrazovke
	int y;

	int w;						//rozmery obrazka na obrazovke
	int h;

	int px;						//suradnice casti surfrace
	int py;

	int pw;						//rozmeri casti surfrace
	int ph;

	int layer;

	image_t *image;				//dane surfrace
} layer_t;

extern bool_t layer_is_inicialized();
extern void layer_init();

/*
 * Add image to queue for rendering
 * *img - image to be rendered
 * x,y - coordinates where to draw the image
 * w,h - width and height of rendered part of image
 * px,py - coordinates of upper left corner of rendered part of image
 * layer - on which layer to draw
 *          (0 - under tuxanci | 1 - same as tuxanci | 2 - over tuxancami)
 */

extern void addLayer(image_t * img,
					 int x, int y, int px, int py, int w, int h, int player);

/*
 * Draws queue on the screen
 */
extern void layer_draw_all();
extern void layer_draw_center(int x, int y);
extern void layer_draw_slpit(int local_x, int local_y, int x, int y, int w,
						   int h);
extern void layer_flush();
extern void layer_quit();

#endif
