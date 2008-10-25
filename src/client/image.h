
#ifndef IMAGE_H

#define IMAGE_H

//#include "image.h"
#include "interface.h"

#define IMAGE_NO_ALPHA	0
#define IMAGE_ALPHA	1

#define IMAGE_GROUP_BASE	"base"
#define IMAGE_GROUP_EXTENDSIO	"extension"
#define IMAGE_GROUP_USER	"user"
#define IMAGE_GROUP_OTHER	"other"

/*
typedef struct image_struct
{
	char *name;
	char *group;
	SDL_Surface *image;
} image_data_t;
*/

typedef struct image_struct
{
    int w;
    int h;
    SDL_Surface *image;
} image_t;

extern bool_t isImageInicialized();
extern void initImageData();
extern image_t *addImageData(char *file, int alpha, char *name, char *group);
extern image_t *getImage(char *group, char *name);
extern void delImage(char *group, char *name);
extern void delAllImageInGroup(char *group);
extern void drawImage(image_t * p, int x, int y, int px, int py, int w,
                      int h);
extern void quitImageData();

#endif
