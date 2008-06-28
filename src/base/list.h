
#ifndef LIST_H

#define LIST_H

#include "base/main.h"

#define LIST_ALLOC_LIMIT 16

typedef struct list_str
{
	void **list;
	int count;
	int alloc;
} list_t;

extern list_t* newList();
extern list_t* cloneList(list_t *p);
extern list_t* cloneListItem(list_t *p, void *f);

extern void addList(list_t *p, void *item);
extern void insList(list_t *p, int n, void *item);
extern void *getList(list_t *p,int n);
extern int searchListItem(list_t *p, void *n);

extern void delList(list_t *p,int n);
extern void delListItem(list_t *p,int n,void *f);
extern void listDoEmpty(list_t *p);

extern void destroyList(list_t *p);
extern void destroyListItem(list_t *p,void *f);

#endif
