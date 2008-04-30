
#include "main.h" 
#include "interface.h" 
#include "string_length.h"
#include "font.h"
#include "path.h"

static TTF_Font *g_font;
static int fontSize;

static bool_t isFontInit = FALSE;

bool_t isFontInicialized()
{
	return isFontInit;
}

/*
 * Inicialuzuje font.
 * nacita font zo suboru *file o  velkosti size
 * Automaticky sa styk nastavy ako TTF_STYLE_BOLD
 */
void initFont(char *file,int size)
{
	char str[STR_PATH_SIZE];

	assert( file != NULL );
	assert( size > 0 );

	assert( isInterfaceInicialized() == TRUE );

	sprintf(str, PATH_FONT "%s", file);

	if( TTF_Init() == -1 )
	{
		fprintf(stderr, "%s\n", SDL_GetError());
		return;
	}

	accessExistFile(str);

	g_font = TTF_OpenFont(str, size);
	TTF_SetFontStyle(g_font, TTF_STYLE_NORMAL);

	fontSize = size;

	printf("init font.. (%s)\n", file);
	isFontInit = TRUE;
}

/*
 * Zobrazi text *s na suradnicu x y s farbou RGB
 */
void drawFont(char *s, int x, int y, int r, int g, int b)
{
	SDL_Rect dst_rect;
	SDL_Surface *text;
	SDL_Surface *p;
	SDL_Color farba_pisma = {r, g, b, 0};

	assert( s != NULL );

	p = getSDL_Screen();

	text = TTF_RenderUTF8_Blended(g_font, s, farba_pisma);

	dst_rect.x = x;
	dst_rect.y = y;
	
	SDL_BlitSurface(text, NULL, p, &dst_rect);
	SDL_FreeSurface(text);
}

/*
 * Vrati velkost daneho fontu.
 */
int getFontSize()
{
	return fontSize;
}

void getTextSize(char *s, int *w, int *h)
{
	assert( s != NULL );
	assert( w != NULL );
	assert( h != NULL );

	TTF_SizeUTF8(g_font, s, w, h);
}

/*
 * Uvolni font z pamete.
 */
void quitFont()
{
	TTF_CloseFont(g_font);
	TTF_Quit();

	printf("quit font\n");
	isFontInit = FALSE;
}
