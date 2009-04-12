
#ifndef LIST_H
#    define LIST_H
#    define LIST_ALLOC_LIMIT 16

#    include "main.h"

typedef struct list_str {
	void **list;
	int count;
	int alloc;
} list_t;

extern list_t *list_new();
extern list_t *list_clone(list_t * p);
extern list_t *list_clone_item(list_t * p, void *f);
extern void list_add(list_t * p, void *item);
extern void list_ins(list_t * p, int n, void *item);
extern void *list_get(list_t * p, int n);
extern int list_search(list_t * p, void *n);
extern void list_del(list_t * p, int n);
extern void list_del_item(list_t * p, int n, void *f);
extern void list_do_empty(list_t * p);
extern void list_destroy(list_t * p);
extern void list_destroy_item(list_t * p, void *f);
#endif
