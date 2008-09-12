
#include <stdlib.h>
#include <assert.h>

#include "main.h"
#include "list.h"
#include "tux.h"

#include "interface.h"
#include "layer.h"
#include "image.h"

#include "screen_world.h"

static list_t *listLayer;

static bool_t isLayerInit = FALSE;

bool_t isLayerInicialized()
{
	return isLayerInit;
}

/*
 * Inicializuje globalny zoznam vsrtiev
 */
void initLayer()
{
	listLayer = newList();
	isLayerInit = TRUE;
}

/*
 * Prida novu polozku do zoznamu vrstiev
 * *img - obrazok, ktory obsahuje polozka
 * x y w h - suradnca kam sa obrazok vykresli a jeho rozmery
 * px py pw ph - suradnica casti obrazka *img ktora sa ma zobrazit
 * layer - vrstva na ktorej sa zobrazi obrazok
 */
void addLayer(image_t *img,
		int x,int y, int px,int py,
		int w,int h, int player)
{
	layer_t *new;
	layer_t *this;
	int i, j;

	assert( img != NULL );

	new = malloc( sizeof(layer_t) );
	new->x = x;
	new->y = y;
	new->w = w;
	new->h = h;
	new->px = px;
	new->py = py;
	new->layer = player;
	new->image = img;

	for( i = 0 ; i < listLayer->count ; i++ )
	{
		this = (layer_t *)listLayer->list[i];
		
		if( this->layer == new->layer )
		{
			for( j = i ; j < listLayer->count ; j++ )
			{
				this = (layer_t *)listLayer->list[j];

				if( this->y+this->h > new->y+new->h )
				{
					insList(listLayer, j, new);
					return;
				}
			}
		}
	}

	addList(listLayer, new);
}

/*
 * Vykresli vsetky polozky na obrazovku
 * poukadane podla vrstvy a suradnice Y+H
 * po dokresleni sa zoznam uvolni.
 */
void drawLayer()
{
	layer_t *this;
	int i;

	for( i = 0 ; i < listLayer->count ; i++ )
	{
		this = (layer_t *)listLayer->list[i];

		drawImage(this->image,
			this->x, this->y,
			this->px, this->py,
			this->w, this->h);
	}

	//printf("listLayer->count = %d\n", listLayer->count);

	destroyListItem(listLayer, free);
	listLayer = newList();
}

void drawLayerCenter(int x, int y)
{
	layer_t *this;
	int screen_x, screen_y;
	int i;
	//int count = 0;

	getCenterScreen(&screen_x, &screen_y, x, y, WINDOW_SIZE_X, WINDOW_SIZE_Y);

	for( i = 0 ; i < listLayer->count ; i++ )
	{
		this = (layer_t *)listLayer->list[i];

		if( this->x + this->w  < screen_x || this->x > screen_x + WINDOW_SIZE_X ||
		    this->y + this->h  < screen_y || this->y > screen_y + WINDOW_SIZE_Y )
		{
			continue;
		}

		//count++;

		drawImage(this->image,
			this->x - screen_x, this->y - screen_y,
			this->px, this->py,
			this->w, this->h);

		//printf("%d %d\n", this->x - screen_x, this->y - screen_y);
	}

	//printf("count = %d\n", count);

	destroyListItem(listLayer, free);
	listLayer = newList();
}

void drawLayerSplit(int local_x, int local_y, int x, int y, int w, int h)
{
	layer_t *this;
	int i;
	//int count = 0;

	for( i = 0 ; i < listLayer->count ; i++ )
	{
		this = (layer_t *)listLayer->list[i];

		if( this->x + this->w  < x || this->x > x + w ||
		    this->y + this->h  < y || this->y > y + h )
		{
			continue;
		}

		//count++;

		if( local_x + (this->x - x) < local_x )
		{
			int z;

			z = local_x - ( local_x + (this->x - x) );

			this->x += z;
			this->px += z;
			this->w -= z;
		}

		if( local_x + (this->x - x) + this->w > local_x+w )
		{
			this->w -= ( local_x + (this->x - x) + this->w ) - (local_x + w);
		}

		if( local_y + (this->y - y) < local_y )
		{
			int z;

			z = local_y - ( local_y + (this->y - y) );

			this->y += z;
			this->py += z;
			this->h -= z;
		}

		if( local_y + (this->y - y) + this->h > local_y+h )
		{
			this->h -= ( local_y + (this->y - y) + this->h ) - (local_y + h);
		}

		drawImage(this->image,
			local_x + (this->x - x), local_y + (this->y - y),
			this->px, this->py,
			this->w, this->h);

		//printf("%d %d\n", this->x - screen_x, this->y - screen_y);
	}

	//printf("count = %d\n", count);

	destroyListItem(listLayer, free);
	listLayer = newList();
}

void flushLayer()
{
	destroyListItem(listLayer, free);
	listLayer = newList();
}

/*
 * Odstrani zoznam vrtsiev z pamete.
 */
void quitLayer()
{
	destroyListItem(listLayer, free);
	isLayerInit = FALSE;
}

