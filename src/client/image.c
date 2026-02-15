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

bool_t image_is_inicialized()
{
	return isImageDataInit;
}

/*
 * Initialization of global list of images
 */
void image_init()
{
	assert(interface_is_inicialized() == TRUE);

	debug("Initializing image database");

	listStorage = storage_new();
	isImageDataInit = TRUE;
}

static SDL_Surface *loadImage(const char *filename, int alpha)
{
	SDL_Surface *tmp;
	SDL_Surface *ret;

	char str[STR_PATH_SIZE];

	if (isFillPath(filename)) {
		strcpy(str, filename);
	} else {
		sprintf(str, PATH_IMAGE "%s", filename);
	}

	accessExistFile(str);

		if ((tmp = IMG_Load(str)) == NULL) {
			error("SDL: %s", SDL_GetError());
			return NULL;
		}

		if (alpha) {
			ret = SDL_ConvertSurfaceFormat(tmp, SDL_PIXELFORMAT_RGBA8888, 0);
		} else {
			ret = SDL_ConvertSurfaceFormat(tmp, SDL_PIXELFORMAT_RGB24, 0);
		}

		if (ret == NULL) {
			error("SDL: %s", SDL_GetError());
			SDL_FreeSurface(tmp);
			return NULL;
		}

		SDL_FreeSurface(tmp);
		return ret;
}


image_t *image_new_sdl(SDL_Surface *surface)
{
	image_t *new;

	assert(surface != NULL);

	new = malloc(sizeof(image_t));
	new->image = surface;
	new->w = surface->w;
	new->h = surface->h;

	return new;
}

#ifdef SUPPORT_OPENGL
/*
 * returns closest bigger power of 2 to i
 */
unsigned int closestpoweroftwo(unsigned int i)
{
	int p;
	p=0;
	while (i) {
		i = i >> 1;
		p++;
	}
	return 1 << (p - 0);
}

/* convert from SDL_Surface to image_t
 * WARNING: image_new is indestructive to surface, caller is responsible for freeing surface
 * surface is not needed for image_t after execution of image_new
 */

image_t *image_new_opengl(SDL_Surface *surface)
{
	image_t *new;
	Uint32 rmask, gmask, bmask, amask;
	SDL_Surface *sdl_rgba_surface;
	unsigned int bpp;

	/* it is unoptimal to blit to buffer surface before actual loading to opengl texture but it is safer and simpler to implement */
	/* it is unoptimal to always use RGBA texture */

	/* we set properties of our buffer sdl_rgba_surface */
	rmask = 0x000000ff;
	gmask = 0x0000ff00;
	bmask = 0x00ff0000;
	amask = 0xff000000;
	bpp = 32;		/* bits per pixel */

	assert(surface != NULL);

	new = malloc(sizeof(image_t));
	new->w = surface->w;
	new->h = surface->h;

	/* because some older hw is really slow when using textures with width and height which is not a power of two */
	new->tw = closestpoweroftwo(new->w);
	new->th = closestpoweroftwo(new->h);

		sdl_rgba_surface = SDL_CreateRGBSurface(0, new->tw, new->th, bpp,  rmask, gmask, bmask, amask);

		SDL_SetSurfaceBlendMode(surface, SDL_BLENDMODE_NONE);
		SDL_BlitSurface(surface, NULL, sdl_rgba_surface, NULL);

	SDL_LockSurface(sdl_rgba_surface);		/* we ensure that we can read pixels from sdl_rgba_surface */

	GLuint tid;
	glGenTextures(1, &tid);				/* we ask for free texture id */
	glBindTexture(GL_TEXTURE_2D, tid);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	/* scale linearly when image bigger than texture */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	/* scale linearly when image smalled than texture */
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, new->tw, new->th, 0, GL_RGBA, GL_UNSIGNED_BYTE, sdl_rgba_surface->pixels);
	SDL_UnlockSurface(sdl_rgba_surface);
	SDL_FreeSurface(sdl_rgba_surface);

	new->tex_id = tid;

	SDL_FreeSurface(surface);

	return new;
}
#endif /* SUPPORT_OPENGL */

image_t *image_new(SDL_Surface *surface)
{
#ifdef SUPPORT_OPENGL
	if (interface_is_use_open_gl()) {
		return image_new_opengl(surface);
	} else {
		return image_new_sdl(surface);
	}
#else /* SUPPORT_OPENGL */
	return image_new_sdl(surface);
#endif /* SUPPORT_OPENGL */
}

void image_destroy(image_t *p)
{
	assert(p != NULL);

#ifndef SUPPORT_OPENGL
	SDL_FreeSurface((SDL_Surface *) p->image);
#else /* SUPPORT_OPENGL */
	if (interface_is_use_open_gl()) {
		glDeleteTextures(1, &p->tex_id);
	} else {
		SDL_FreeSurface((SDL_Surface *) p->image);
	}
#endif /* SUPPORT_OPENGL */

	free(p);
}

/*
 * Adds an image to the global image list
 * *file - name of the image file
 * *name - name of the image (is used for searching in the list)
 * alpha - [0] without alpha channel; [1] with alpha channel
 */
image_t *image_add(char *file, int alpha, char *name, char *group)
{
	SDL_Surface *surface;
	image_t *new;

	assert(file != NULL);
	assert(name != NULL);
	assert(group != NULL);

	surface = loadImage(file, alpha);
	new = image_new(surface);

	storage_add(listStorage, group, name, new);

	debug("Loading image [%s]", file);

	return new;
}

/*
 * Returns pointer to image with name *name in the global image list
 */
image_t *image_get(char *group, char *name)
{
	assert(group != NULL);
	assert(name != NULL);

	return storage_get(listStorage, group, name);
}

void image_del(char *group, char *name)
{
	assert(group != NULL);
	assert(name != NULL);

	storage_del(listStorage, group, name, image_destroy);
}

void image_del_all_image_in_group(char *group)
{
	assert(group != NULL);

	storage_del_all(listStorage, group, image_destroy);
}

void image_draw_sdl(image_t *p, int x, int y, int px, int py, int w, int h)
{
	static SDL_Surface *screen = NULL;
	SDL_Rect dst_rect, src_rect;

	if (screen == NULL) {
		screen = interface_get_screen();
	}

	dst_rect.x = x;
	dst_rect.y = y;

	src_rect.x = px;
	src_rect.y = py;
	src_rect.w = w;
	src_rect.h = h;

	SDL_BlitSurface(p->image, &src_rect, screen, &dst_rect);
}

#ifdef SUPPORT_OPENGL
/*
 * Draws image on screen at [x,y], with width w and height h, top-left corner on image is at [px,py]
 */
void image_draw_opengl(image_t *image, int x,int y, int px, int py, int w, int h)
{
	/* x - coordinate of left border; xx - coordinate of right border
	 * y - coordinate of top border; yy - coordinate of bottom border
	 * t - texture space; d - screen space
	 */
	float t_x, t_y, t_xx, t_yy;
	float d_x, d_y, d_xx, d_yy;

	/* screen space */
	d_x = x;
	d_y = y;
	d_xx = x+w;
	d_yy = y+h;

	/* texture space (remember that texture space is from 0.0  to 1.0) */
	t_x = px / (float) image->tw;
	t_y = py / (float) image->th;
	t_xx = (px+w) / (float) image->tw;
	t_yy = (py+h) / (float) image->th;

	glBindTexture(GL_TEXTURE_2D, image->tex_id);

	/*
	 * 2--4
	 * |\ |
	 * | \|
	 * 1--3
	 */
	glBegin(GL_TRIANGLE_STRIP);
		/* 1 */
		glTexCoord2f(t_x,t_yy);
		glVertex2f(d_x, d_yy);
		/* 2 */
		glTexCoord2f(t_x, t_y);
		glVertex2f(d_x, d_y);
		/* 3 */
		glTexCoord2f(t_xx, t_yy);
		glVertex2f(d_xx, d_yy);
		/* 4 */
		glTexCoord2f(t_xx,t_y);
		glVertex2f(d_xx, d_y);
	glEnd();
}
#endif /* SUPPORT_OPENGL */

void image_draw(image_t *image, int x,int y, int px, int py, int w, int h)
{
#ifdef SUPPORT_OPENGL
	if (interface_is_use_open_gl()) {
		image_draw_opengl(image, x, y, px, py, w, h);
	} else {
		image_draw_sdl(image, x, y, px, py, w, h);
	}
#else /* SUPPORT_OPENGL */
	image_draw_sdl(image, x, y, px, py, w, h);
#endif /* SUPPORT_OPENGL */
}

/*
 * Frees global image list
 */
void image_quit()
{
	debug("Shutting down image database");

	storage_destroy(listStorage, image_destroy);
	isImageDataInit = FALSE;
}
