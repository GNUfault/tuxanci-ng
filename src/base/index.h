
#ifndef INDEX_H

#define INDEX_H

#include "main.h"
#include "list.h"

extern list_t* newIndex();
extern int addToIndex(list_t *list, int index);
extern int getFormIndex(list_t *list, int index);
extern void delFromIndex(list_t *list, int index);
extern void destroyIndex(list_t *list);

#endif
