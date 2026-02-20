#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "main.h"
#include "list.h"

list_t *list_new()
{
	list_t *new;

	new = malloc(sizeof(list_t));
	memset(new, 0, sizeof(list_t));

	return new;
}

list_t *list_clone(list_t *p)
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

list_t *list_clone_item(list_t *p, void *f)
{
	list_t *new;
	void *(*fce) (void *);
	int i;

	assert(p != NULL);
	assert(f != NULL);
	new = list_clone(p);
	fce = f;

	for (i = 0; i < p->count; i++) {
		new->list[i] = fce(p->list[i]);
	}

	return new;
}

void list_add(list_t *p, void *item)
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
#ifdef DEBUG_LIST
		printf("[Debug] Reallocating list from %d to %d (count: %d)\n",
		       p->alloc, p->alloc + LIST_ALLOC_LIMIT, p->count);
#endif
		p->alloc += LIST_ALLOC_LIMIT;
		p->list = realloc(p->list, p->alloc * sizeof(void *));
		p->list[p->count] = item;
		p->count++;
		return;
	}
}

void list_ins(list_t *p, int n, void *item)
{
	assert(p != NULL);
	list_add(p, NULL);		/* :) */
	assert(n >= 0 || n < p->count);
	memmove(p->list + n + 1, p->list + n, ((p->count - 1) - n) * sizeof(void *));
	p->list[n] = item;
}

void *list_get(list_t *p, int n)
{
	assert(p != NULL);
	assert(n >= 0 || n < p->count);
	return p->list[n];
}

int list_search(list_t *p, void *n)
{
	int i;

	assert(p != NULL);

	for (i = 0; i < p->count; i++) {
		if (p->list[i] == n) {
			return i;
		}
	}

	return -1;
}

void list_del(list_t *p, int n)
{
	assert(p != NULL);
	assert(n >= 0 || n < p->count);

	memmove(p->list + n, p->list + n + 1, ((p->count - 1) - n) * sizeof(void *));
	p->count--;

	if (p->count + LIST_ALLOC_LIMIT < p->alloc) {
		void **new;

#ifdef DEBUG_LIST
		printf("Reallocating list from %d to %d (count: %d)\n",
		       p->alloc, p->alloc - LIST_ALLOC_LIMIT, p->count);
#endif
		p->alloc -= LIST_ALLOC_LIMIT;
		new = malloc(p->alloc * sizeof(void *));
		memcpy(new, p->list, p->count * sizeof(void *));
		free(p->list);
		p->list = new;
	}
}

void list_del_item(list_t *p, int n, void *f)
{
    void (*fce)(void *) = (void (*)(void *))f;

    assert(p != NULL);
    assert(n >= 0 || n < p->count);

    if (fce != NULL) {
        fce(p->list[n]);
    }

	list_del(p, n);
}

void list_do_empty(list_t *p)
{
	p->count = 0;
}

void list_destroy(list_t *p)
{
	assert(p != NULL);

	if (p->list != NULL) {
		free(p->list);
	}

	free(p);
}

void list_destroy_item(list_t *p, void *f)
{
	void (*fce) (void *);
	int i;

	assert(p != NULL);
	assert(f != NULL);

	fce = f;

	for (i = 0; i < p->count; i++) {
		fce(p->list[i]);
	}

	list_destroy(p);
}
