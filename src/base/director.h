
#ifndef DIRECTOR_H
#define DIRECTOR_H

#include "main.h"
#include "list.h"

typedef struct director_struct
{
    char *path;
    list_t *list;
} director_t;

extern director_t *loadDirector(char *s);
extern void destroyDirector(director_t * p);
#endif
