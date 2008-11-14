#include "main.h"
#include "interface.h"
#include "font.h"
#include "image.h"

static TTF_Font *g_font;
static int fontSize;

static bool_t isFontInit = FALSE;

bool_t isFontInicialized()
{
	return isFontInit;
}

void initFont(char *file, int size)
{
	char str[STR_PATH_SIZE];

	assert(file != NULL);
	assert(size > 0);

	assert(isInterfaceInicialized() == TRUE);

	sprintf(str, "%s", file);

	if (TTF_Init() == -1) {
		fprintf(stderr, "%s\n", SDL_GetError());
		return;
	}

	accessExistFile(str);

	g_font = TTF_OpenFont(str, size);
	TTF_SetFontStyle(g_font, TTF_STYLE_NORMAL);

	fontSize = size;

	DEBUG_MSG(_("Loading font: \"%s\"\n"), file);

	isFontInit = TRUE;
}

/*
 * Zobrazi text *string na suradnicu x y s farbou RGB
 */
void drawFont(char *string, int x, int y, int r, int g, int b)
{
	SDL_Surface *text;
	image_t *image;
	SDL_Color font_color = {r, g, b, SDL_ALPHA_OPAQUE};


	assert( string != NULL );


	text = TTF_RenderUTF8_Blended(g_font, string, font_color);

	// because if string=="" TTF_RenderUTF8_Blended returns NULL
	if( text != NULL ){
		image = newImage(text);
		drawImage(image, x, y, 0, 0, image->w, image->h);
		destroyImage(image);
	};
}

void drawFontMaxSize(char *s, int x, int y, int w, int h, int r, int g, int b)
{
	SDL_Rect src_rect, dst_rect;
	SDL_Surface *text;
	image_t *i;
	SDL_Color font_color = {r, g, b, SDL_ALPHA_OPAQUE};
	int my_w, my_h;

	assert(s != NULL);


	text = TTF_RenderUTF8_Blended(g_font, s, font_color);


	my_w = text->w;

	if (my_w > w) {
		my_w = w;
	}

	my_h = text->h;

	if (my_w > w) {
		my_h = h;
	}

	src_rect.x = 0;
	src_rect.y = 0;
	src_rect.w = my_w;
	src_rect.h = my_h;

	dst_rect.x = x;
	dst_rect.y = y;

	i = newImage(text);

	//posibly broken
	drawImage(i, x, y, 0, 0, my_w, my_h);
	destroyImage(i);
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
	assert(s != NULL);
	assert(w != NULL);
	assert(h != NULL);

	TTF_SizeUTF8(g_font, s, w, h);
}

/*
 * Uvolni font z pamete.
 */
void quitFont()
{
	TTF_CloseFont(g_font);
	TTF_Quit();

	DEBUG_MSG(_("Unloading font\n"));

	isFontInit = FALSE;
}
