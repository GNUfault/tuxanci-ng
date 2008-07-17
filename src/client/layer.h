
#ifndef MY_LAYER_H

#define MY_LAYER_H

#include "main.h"
#include "image.h"

typedef struct layer_str
{
	int x; //suradnice obrazka na obrazovke
	int y;

	int w; //rozmery obrazka na obrazovke
	int h;

	int px; //suradnice casti surfrace
	int py;

	int pw; //rozmeri casti surfrace
	int ph;

	int layer;

	image_t *image; //dane surfrace
} layer_t;

extern bool_t isLayerInicialized();
extern void initLayer();

/*
 * Prida obrazok na vykreslenie do zoznamu
 * *img - ponter na urfrace obrazka
 * x,y - suradnice kam sa ma obrazok nakreslit
 * w,h - rozmeri obrazka
 * px,py - suradnice casti *img z ktorej sa bude obrazok vykreslovat
 * ph,ph - rozmeri casti obrazka *img z ktorej sa bude obrazok vykreslovat
 * layer - layer, na ktorej bude obrazok 
 *          (0 - pod tuxancami | 1 - na tej istej ako tuxanci | 2 - nad tuxancami)
 */

extern void addLayer(image_t *img,
	int x,int y, int px,int py,
	int w,int h, int player);

/*
 * Vykresli zoznam na obrazovku
 */
extern void drawLayer();
extern void drawLayerCenter(int x, int y);
extern void flushLayer();
extern void quitLayer();

#endif
