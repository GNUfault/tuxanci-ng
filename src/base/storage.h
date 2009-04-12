
#ifndef STORAGE_H
#    define STORAGE_H

#    include "main.h"
#    include "list.h"

extern list_t *storage_new();
extern void storage_add(list_t * list, char *group, char *name,
							 void *data);
extern void *storage_get(list_t * list, char *group, char *name);
extern void storage_del(list_t * list, char *group, char *name,
							   void *f);
extern void storage_del_all(list_t * list, char *group, void *f);
extern void storage_destroy(list_t * p, void *f);
#endif
