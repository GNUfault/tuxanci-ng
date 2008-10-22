
#ifndef SPACE_H

#define SPACE_H

#include "main.h"
#include "list.h"

typedef struct zone_struct
{
    list_t *list;
} zone_t;

typedef struct space_struct
{
    int w;
    int h;

    int segW;
    int segH;

    zone_t ***zone;

    list_t *listIndex;

    void (*getStatus) (void *p, int *id, int *x, int *y, int *w, int *h);
    void (*setStatus) (void *p, int x, int y, int w, int h);
} space_t;

extern space_t *newSpace(int w, int h, int segW, int segH,
                         void (*getStatus) (void *p, int *id, int *x, int *y,
                                            int *w, int *h),
                         void (*setStatus) (void *p, int x, int y, int w,
                                            int h));

extern int getSpaceCount(space_t * p);
extern void *getItemFromSpace(space_t * p, int offset);
extern void addObjectToSpace(space_t * p, void *item);
extern void getObjectFromSpace(space_t * p, int x, int y, int w, int h,
                               list_t * list);
extern void *getObjectFromSpaceWithID(space_t * p, int id);
extern int isConflictWithObjectFromSpace(space_t * p, int x, int y, int w,
                                         int h);
extern int isConflictWithObjectFromSpaceBut(space_t * p, int x, int y, int w,
                                            int h, void *but);
extern void delObjectFromSpace(space_t * p, void *item);
extern void delObjectFromSpaceWithObject(space_t * p, void *item, void *f);
extern void moveObjectInSpace(space_t * p, void *item, int move_x,
                              int move_y);
extern void printSpace(space_t * p);
extern void actionSpace(space_t * space, void *f, void *p);
extern void actionSpaceFromLocation(space_t * space, void *f, void *p, int x,
                                    int y, int w, int h);
extern void destroySpace(space_t * p);
extern void destroySpaceWithObject(space_t * p, void *f);

#endif
