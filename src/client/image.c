
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "main.h"
#include "list.h"
#include "storage.h"

#include "interface.h"
#include "image.h"

static list_t *listStorage;

static bool_t isImageDataInit = FALSE;

bool_t isImageInicialized()
{
	return isImageDataInit;
}

/*
 * Inicializuje globalny zoznam obrazkov
 */
void initImageData()
{
	assert( isInterfaceInicialized() == TRUE );

	printf("init image database..\n");
	listStorage = newStorage();
	isImageDataInit = TRUE;
}

static SDL_Surface *loadImage(const char *filename, int alpha)
{
	SDL_Surface *tmp;
	SDL_Surface *ret;

	char str[STR_PATH_SIZE];


	sprintf(str, PATH_IMAGE "%s", filename);
	
	accessExistFile(str);

	if( ( tmp = IMG_Load(str) ) == NULL )
	{
		fprintf(stderr, "%s\n", SDL_GetError());
		return NULL;
	}

	if( ( ret = (alpha) ? SDL_DisplayFormatAlpha(tmp) : SDL_DisplayFormat(tmp) ) == NULL )
	{
		fprintf(stderr, "%s\n", SDL_GetError());
		SDL_FreeSurface(tmp);
		return NULL;
	}

	SDL_FreeSurface(tmp);
	return ret;
}

image_t* newImage(SDL_Surface *surface)
{
	image_t *new;

	assert( surface != NULL );

	new = malloc( sizeof(image_t) );
	new->image = surface;
	new->w = surface->w;
	new->h = surface->h;

	return new;
}

static void destroyImage(image_t *p)
{
	assert( p != NULL );
	SDL_FreeSurface((SDL_Surface *)p->image);
	free(p);
}

/*
 * Prida obrazok do globalneho zoznamu obrazkov
 * *file - nazov suboru v ktorom je obrazok ulozeny
 * *name - nazov obrazku ( pouzije sa ako vyhadavaci retazec v zazanem )
 * alpha - 0 - nema alpha kanal | 1 - ma alpha kanal
 */
image_t* addImageData(char *file, int alpha, char *name, char *group)
{
	SDL_Surface *surface;
	image_t *new;

	assert( file != NULL );
	assert( name != NULL );
	assert( group != NULL );

	surface = loadImage(file, alpha);
	new = newImage(surface);

	addItemToStorage(listStorage, group, name, new );

	printf("load image %s\n", file);

	return new;
}

/*
 * Vrati odkaz na image_data v globalnom zozname obrazkov
 * a nazvom *s
 */
image_t* getImage(char *group, char *name)
{
	assert( group != NULL );
	assert( name != NULL );

	return getItemFromStorage(listStorage, group, name);
}

void delImage(char *group, char *name)
{
	assert( group != NULL );
	assert( name != NULL );
	
	delItemFromStorage(listStorage, group, name, destroyImage);
}

void delAllImageInGroup(char *group)
{
	assert( group != NULL );

	delAllItemFromStorage(listStorage, group, destroyImage);
}

void drawImage(image_t *p, int x,int y, int px, int py, int w, int h)
{
	static SDL_Surface *screen = NULL;
	SDL_Rect dst_rect, src_rect;

	if( screen == NULL )
	{
		screen = getSDL_Screen();
	}

	dst_rect.x = x;
	dst_rect.y = y;

	src_rect.x = px;
	src_rect.y = py;
	src_rect.w = w;
	src_rect.h = h;

	SDL_BlitSurface(p->image, &src_rect, screen, &dst_rect);
}

/*
 * Odstrani zoznam obrazkov z pamate
 */
void quitImageData()
{
	printf("quit image database..\n");
	destroyStorage(listStorage, destroyImage);
	isImageDataInit = FALSE;
}
 
