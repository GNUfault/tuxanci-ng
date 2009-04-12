
#ifndef DIRECTOR_H
#    define DIRECTOR_H

#    include "main.h"
#    include "list.h"

typedef struct director_struct {
	char *path;
	list_t *list;
} director_t;

extern director_t *director_load(char *s);
extern void director_destroy(director_t * p);
#endif
