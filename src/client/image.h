
#ifndef IMAGE_H

#    define IMAGE_H

#    include "main.h"
#    include "interface.h"

#    define IMAGE_NO_ALPHA	0
#    define IMAGE_ALPHA	1

#    define IMAGE_GROUP_BASE	"base"
#    define IMAGE_GROUP_EXTENDSIO	"extension"
#    define IMAGE_GROUP_USER	"user"
#    define IMAGE_GROUP_OTHER	"other"

#ifdef SUPPORT_OPENGL
#include <SDL_opengl.h>
#endif

/*
typedef struct image_struct
{
	char *name;
	char *group;
	SDL_Surface *image;
} image_data_t;
*/

#ifndef SUPPORT_OPENGL
typedef struct image_struct {
	int w;
	int h;
	SDL_Surface *image;
} image_t;
#endif

#ifdef SUPPORT_OPENGL
typedef struct image_struct
{
	int w; // width of actual picture
	int h; // height of actual picture
	GLuint tw; // width of allocated texture
	GLuint th; // height of allocated texture
	GLuint tex_id; // texture id
} image_t;
#endif

extern bool_t image_is_inicialized();
extern void image_init();
extern image_t* image_new(SDL_Surface *surface);
extern void image_destroy(image_t * p);
extern image_t *image_add(char *file, int alpha, char *name, char *group);
extern image_t *image_get(char *group, char *name);
extern void image_del(char *group, char *name);
extern void image_del_all_image_in_group(char *group);
extern void image_draw(image_t * p, int x, int y, int px, int py, int w, int h);
extern void image_quit();

#endif
