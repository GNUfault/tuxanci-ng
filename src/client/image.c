
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "base/main.h"
#include "base/list.h"
#include "base/storage.h"

#include "client/interface.h"
#include "client/image.h"

static list_t *listStorage;

static bool_t isImageDataInit = FALSE;

bool_t isImageInicialized()
{
	return isImageDataInit;
}

/*
 * Inicialuzje globalny zoznam obrazkov
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

static void destroySDLSurface(void *p)
{
	assert( p != NULL );
	SDL_FreeSurface((SDL_Surface *)p);
}

/*
 * Prida obrazok do globalneho zoznamu obrazkov
 * *file - nazov suboru v ktorom je obrazok ulozeny
 * *name - nazov obrazku ( pouzije sa ako vyhadavaci retazec v zazanem )
 * alpha - 0 - nema alpha kanal | 1 - ma alpha kanal
 */
SDL_Surface* addImageData(char *file, int alpha, char *name, char *group)
{
	SDL_Surface *new;

	assert( file != NULL );
	assert( name != NULL );
	assert( group != NULL );

	new = loadImage(file, alpha);

	addItemToStorage(listStorage, group, name, new );

	printf("load image %s\n", file);

	return new;
}

/*
 * Vrati odkaz na image_data v globalnom zozname obrazkov
 * a nazvom *s
 */
SDL_Surface* getImage(char *group, char *name)
{
	assert( group != NULL );
	assert( name != NULL );

	return getItemFromStorage(listStorage, group, name);
}

void delImage(char *group, char *name)
{
	assert( group != NULL );
	assert( name != NULL );
	
	delItemFromStorage(listStorage, group, name, destroySDLSurface);
}

void delAllImageInGroup(char *group)
{
	assert( group != NULL );

	delAllItemFromStorage(listStorage, group, destroySDLSurface);
}

void drawImage(SDL_Surface *p, int x,int y, int px, int py, int w, int h)
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
		
	SDL_BlitSurface(p, &src_rect, screen, &dst_rect);
}

/*
 * Odstrani zoznam obrazkov z pamate
 */
void quitImageData()
{
	printf("quit image database..\n");
	destroyStorage(listStorage, destroySDLSurface);
	isImageDataInit = FALSE;
}
 
