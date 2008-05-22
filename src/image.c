
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "main.h"
#include "interface.h"
#include "image.h"
#include "list.h"
#include "path.h"
#include "string_length.h"

static list_t *listImageData;

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
	listImageData = newList();
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

/*
 * Prida obrazok do globalneho zoznamu obrazkov
 * *file - nazov suboru v ktorom je obrazok ulozeny
 * *name - nazov obrazku ( pouzije sa ako vyhadavaci retazec v zazanem )
 * alpha - 0 - nema alpha kanal | 1 - ma alpha kanal
 */
SDL_Surface* addImageData(char *file, int alpha, char *name, char *group)
{
	image_data_t *new;

	assert( file != NULL );
	assert( name != NULL );
	assert( group != NULL );

	new = malloc(sizeof(image_data_t));

	new->image = loadImage(file, alpha);
	//if(strstr(file,"BMP")!=NULL)SDL_SetColorKey(new->image, SDL_SRCCOLORKEY, SDL_MapRGB(new->image->format, 255, 255, 255));
	new->name = strdup(name);
	new->group = strdup(group);

	addList(listImageData, new);

	printf("load image %s\n", file);
	return new->image;
}

static void destroyImageData(image_data_t *p)
{
	assert( p != NULL );

	free(p->name);
	free(p->group);
	SDL_FreeSurface(p->image);
	free(p);
}

/*
 * Vrati odkaz na image_data v globalnom zozname obrazkov
 * a nazvom *s
 */
SDL_Surface* getImage(char *group, char *name)
{
	image_data_t *this;

	assert( group != NULL );
	assert( name != NULL );

	int i;
	
	for(i = 0 ; i < listImageData->count; i++)
	{
		this = (image_data_t *) listImageData->list[i];
	
		if( strcmp(group, this->group) == 0 && strcmp(name, this->name) == 0 )
		{
			return this->image;
		}
	}

	return NULL;
}

void delImage(char *group, char *name)
{
	image_data_t *this;
	int i;

	assert( group != NULL );
	assert( name != NULL );
	
	for(i = 0 ; i < listImageData->count; i++)
	{
		this = (image_data_t *) listImageData->list[i];
	
		if( strcmp(group, this->group) == 0 && strcmp(name, this->name) == 0 )
		{
			delListItem(listImageData, i, destroyImageData);
			break;
		}

	}
}

void delAllImageInGroup(char *group)
{
	image_data_t *this;
	int i;

	assert( group != NULL );
	printf("del all image in group %s\n", group);
	
	for(i = 0 ; i < listImageData->count; i++)
	{
		this = (image_data_t *) listImageData->list[i];

		if( strcmp(group, this->group) == 0 )
		{
			delListItem(listImageData, i, destroyImageData);
			i--;
		}

	}
}

void drawImage(SDL_Surface *p, int x,int y, int px, int py, int w, int h)
{
	SDL_Rect dst_rect, src_rect;
	SDL_Surface *screen;

	screen = getSDL_Screen();

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
	destroyListItem(listImageData, destroyImageData);
	isImageDataInit = FALSE;
}
 
