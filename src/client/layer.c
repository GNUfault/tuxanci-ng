
#include <stdlib.h>
#include <assert.h>

#include "base/main.h"
#include "base/list.h"
#include "base/tux.h"

#include "client/interface.h"
#include "client/layer.h"
#include "client/image.h"

#include "screen/screen_world.h"

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
}

/*
 * Prida novu polozku do zoznamu vrstiev
 * *img - obrazok, ktory obsahuje polozka
 * x y w h - suradnca kam sa obrazok vykresli a jeho rozmery
 * px py pw ph - suradnica casti obrazka *img ktora sa ma zobrazit
 * layer - vrstva na ktorej sa zobrazi obrazok
 */
void addLayer(SDL_Surface *img,
		int x,int y, int px,int py,
		int w,int h, int player)
{
	layer_t *new;
	layer_t *this;
	int i;

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

	if( listLayer->count == 0 )
	{
		addList(listLayer, new);
		return;
	}

	i = 0;
	this = (layer_t *)listLayer->list[0];

	//vyhladaj svoju vrstvu
	while( i < listLayer->count && this->layer < new->layer )
	{
		this = (layer_t *)listLayer->list[++i];
	}

	//porovnaj s vyskami na tvojej vrstve,
	//ak neexistuje, rovno to tam hod
	this = (layer_t *)listLayer->list[i];

	while( i < listLayer->count && this->y+this->h < new->y+new->h && new->layer == this->layer )
	{
		this = (layer_t *)listLayer->list[++i];
	}

	insList(listLayer, i, new);
	isLayerInit = TRUE;
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

	destroyListItem(listLayer, free);
	listLayer = newList();
}

void drawLayerCenter(int x, int y)
{
	layer_t *this;
	int screen_x, screen_y;
	int i;

	getCenterScreen(&screen_x, &screen_y, x, y);

	for( i = 0 ; i < listLayer->count ; i++ )
	{
		this = (layer_t *)listLayer->list[i];

		if( (this->x - screen_x) + this->w < 0 || (this->x - screen_x) > WINDOW_SIZE_X ||
		    (this->y - screen_y) + this->h < 0 || (this->y - screen_y) > WINDOW_SIZE_Y )
		{
			continue;
		}

		drawImage(this->image,
			this->x - screen_x, this->y - screen_y,
			this->px, this->py,
			this->w, this->h);

		//printf("%d %d\n", this->x - screen_x, this->y - screen_y);
	}

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

