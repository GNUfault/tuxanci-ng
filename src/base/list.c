
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "main.h"
#include "list.h"

list_t *
newList()
{
    list_t *new;

    new = malloc(sizeof(list_t));
    memset(new, 0, sizeof(list_t));
    return new;
}

list_t *
cloneList(list_t * p)
{
    list_t *new;

    assert(p != NULL);
    new = malloc(sizeof(list_t));
    memcpy(new, p, sizeof(list_t));
    if (p->list != NULL) {
        new->list = malloc(p->alloc * sizeof(void *));
        memcpy(new->list, p->list, p->alloc * sizeof(void *));
    }
    return new;
}

list_t *
cloneListItem(list_t * p, void *f)
{
    list_t *new;
    void *(*fce) (void *);
    int i;

    assert(p != NULL);
    assert(f != NULL);
    new = cloneList(p);
    fce = f;
    for (i = 0; i < p->count; i++)
        new->list[i] = fce(p->list[i]);
    return new;
}

void
addList(list_t * p, void *item)
{
    assert(p != NULL);

    if (p->alloc == 0) {
        p->alloc = LIST_ALLOC_LIMIT;
        p->count = 1;
        p->list = malloc(p->alloc * sizeof(void *));
        p->list[0] = item;
        return;
    }

    if (p->count + 1 <= p->alloc) {
        p->list[p->count] = item;
        p->count += 1;
        return;
    }

    if (p->count + 1 > p->alloc) {
        void **new;
#ifdef DEBUG_LIST
        printf("Realocating from %d to %d (count=%d)\n",
               p->alloc, p->alloc + LIST_ALLOC_LIMIT, p->count);
#endif
        p->alloc += LIST_ALLOC_LIMIT;
        new = malloc(p->alloc * sizeof(void *));
        memcpy(new, p->list, p->count * sizeof(void *));
        free(p->list);
        p->list = new;
        p->list[p->count] = item;
        p->count++;
        return;
    }
}

void
insList(list_t * p, int n, void *item)
{
    assert(p != NULL);
    addList(p, NULL);           // :)
    assert(n >= 0 || n < p->count);
    memmove(p->list + n + 1, p->list + n,
            ((p->count - 1) - n) * sizeof(void *));
    p->list[n] = item;
}

void *
getList(list_t * p, int n)
{
    assert(p != NULL);
    assert(n >= 0 || n < p->count);
    return p->list[n];
}

int
searchListItem(list_t * p, void *n)
{
    int i;

    assert(p != NULL);
    for (i = 0; i < p->count; i++)
        if (p->list[i] == n)
            return i;
    return -1;
}

void
delList(list_t * p, int n)
{
    assert(p != NULL);
    assert(n >= 0 || n < p->count);
    memmove(p->list + n, p->list + n + 1,
            ((p->count - 1) - n) * sizeof(void *));
    p->count--;
    if (p->count + LIST_ALLOC_LIMIT < p->alloc) {
        void **new;

#ifdef DEBUG_LIST
        printf("Realocating from %d to %d (count=%d)\n",
               p->alloc, p->alloc - LIST_ALLOC_LIMIT, p->count);
#endif
        p->alloc -= LIST_ALLOC_LIMIT;
        new = malloc(p->alloc * sizeof(void *));
        memcpy(new, p->list, p->count * sizeof(void *));
        free(p->list);
        p->list = new;
    }
}

void
delListItem(list_t * p, int n, void *f)
{
    int (*fce) (void *);

    assert(p != NULL);
    assert(n >= 0 || n < p->count);
    fce = f;
    if (fce != NULL)
        fce(p->list[n]);
    delList(p, n);
}

void
listDoEmpty(list_t * p)
{
    p->count = 0;
}

void
destroyList(list_t * p)
{
    assert(p != NULL);

    if (p->list != NULL)
        free(p->list);
    free(p);
}

void
destroyListItem(list_t * p, void *f)
{
    void (*fce) (void *);
    int i;

    assert(p != NULL);
    assert(f != NULL);
    fce = f;
    for (i = 0; i < p->count; i++)
        fce(p->list[i]);
    destroyList(p);
}
