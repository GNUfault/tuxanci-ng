
#ifndef IMAGE_H

#define IMAGE_H

#include "client/image.h"
#include "client/interface.h"

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

extern bool_t isImageInicialized();
extern void initImageData();
extern SDL_Surface* addImageData(char *file, int alpha, char *name, char *group);
extern SDL_Surface* getImage(char *group, char *name);
extern void delImage(char *group, char *name);
extern void delAllImageInGroup(char *group);
extern void drawImage(SDL_Surface *p, int x,int y, int px, int py, int w, int h);
extern void quitImageData();

#endif
