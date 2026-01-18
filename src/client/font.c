#include "main.h"
#include "interface.h"
#include "font.h"
#include "image.h"
#include "fontConfig.h"

static TTF_Font *g_font;
static int fontSize;

static bool_t isFontInit = FALSE;

bool_t font_is_inicialized()
{
	return isFontInit;
}

void font_init()
{
	char *arg[] = {":lang=he:outline=true:style=Book", "family", "style", "weight", "file", NULL};
	char *font_file;
	font_config_t *font_list;
	int res;

	assert(interface_is_inicialized() == TRUE);

	res = fontconfig_init();

	if (res != 0) {
		return;
	}

	font_list = fontconfig_find(arg);
/*
	int i;
	for(i = 0 ; font_list[i].path != NULL ; i++ )
	{
		printf("path[%d]=%s\nflag[%d]=%s\n\n", i, font_list[i].path, i, font_list[i].flag);
	}
*/
 
	// Make font checking more robust, by GNUfault
	if (font_list == NULL || font_list[0].path == NULL) {
        error("Unable to locate a font");
        return;
    }

	font_file = strdup(font_list[0].path);
	fontSize = FONT_SIZE;

	fontconfig_del_list(font_list);
	fontconfig_quit();

	if (TTF_Init() == -1) {
		free(font_file);
		error("SDL: %s", SDL_GetError());
		return;
	}

	accessExistFile(font_file);

	g_font = TTF_OpenFont(font_file, fontSize);
	TTF_SetFontStyle(g_font, TTF_STYLE_NORMAL);

	debug("Loading font [%s]", font_file);

	free(font_file);
	isFontInit = TRUE;
}

/*
 * Shows text *string on coordinates [x,y] with RGB color
 */
void font_draw(char *string, int x, int y, int r, int g, int b)
{
	SDL_Surface *text;
	image_t *image;
	SDL_Color font_color = {r, g, b, SDL_ALPHA_OPAQUE};

	assert( string != NULL );

	text = TTF_RenderUTF8_Blended(g_font, string, font_color);

	/* because if string=="" TTF_RenderUTF8_Blended returns NULL */
	if (text != NULL) {
		image = image_new(text);
		image_draw(image, x, y, 0, 0, image->w, image->h);
		image_destroy(image);
	}
}

void font_drawMaxSize(char *s, int x, int y, int w, int h, int r, int g, int b)
{
	SDL_Rect src_rect, dst_rect;
	(void)src_rect; /* Fixed by GNUfault */
	(void)dst_rect; /* Fixed by GNUfault */
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

	i = image_new(text);

	/* possibly broken */
	image_draw(i, x, y, 0, 0, my_w, my_h);
	image_destroy(i);
}

/*
 * Returns size of the font
 */
int font_get_size()
{
	return fontSize;
}

void font_text_size(char *s, int *w, int *h)
{
	assert(s != NULL);
	assert(w != NULL);
	assert(h != NULL);

	TTF_SizeUTF8(g_font, s, w, h);
}

/*
 * Frees font from memory
 */
void font_quit()
{
	TTF_CloseFont(g_font);
	TTF_Quit();

	debug("Unloading font");

	isFontInit = FALSE;
}
