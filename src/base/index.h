
#ifndef INDEX_H

#define INDEX_H

#include "main.h"
#include "list.h"

typedef struct index_item_struct
{
    int key;
    void *data;
} index_item_t;

extern list_t *newIndex();
extern void addToIndex(list_t * list, int key, void *data);
extern void *getFromIndex(list_t * list, int key);
extern void delFromIndex(list_t * list, int key);
extern void delFromIndexWithObject(list_t * list, int key, void *f);
extern void actionIndexWithObject(list_t * list, void *f);
extern void destroyIndex(list_t * list);
extern void destroyIndexWithObject(list_t * list, void *f);

#endif
