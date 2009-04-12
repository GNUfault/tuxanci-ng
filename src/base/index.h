
#ifndef INDEX_H

#    define INDEX_H

#    include "main.h"
#    include "list.h"

typedef struct index_item_struct {
	int key;
	void *data;
} index_item_t;

extern list_t *index_new();
extern void index_add(list_t * list, int key, void *data);
extern void *index_get(list_t * list, int key);
extern void index_del(list_t * list, int key);
extern void index_del_with_object(list_t * list, int key, void *f);
extern void index_action(list_t * list, void *f);
extern void index_destroy(list_t * list);
extern void index_destroyWithObject(list_t * list, void *f);

#endif
