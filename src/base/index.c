#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "main.h"
#include "list.h"
#include "index.h"

#define DEBUG_INDEX

static index_item_t *index_item_new(int key, void *data)
{
	index_item_t *new;

	new = malloc(sizeof(index_item_t));
	new->key = key;
	new->data = data;

	return new;
}

#ifdef DEBUG_INDEX
static void printIndexItem(index_item_t *p)
{
	printf(_("[Debug] Index [%d] contains: %p\n"), p->key, p->data);
}

static void printListIndexItem(list_t *list)
{
	int i;

	printf(_("List:\n"));

	for (i = 0; i < list->count; i++) {
		index_item_t *this;

		this = (index_item_t *) list->list[i];
		printIndexItem(this);
	}
}

static void checkList(list_t *list)
{
	int i;
	int prev;
	int this;

	/*return;*/

	if (list->count == 0) {
		printf(_("Nothing\n"));
		return;
	}

	prev = ((index_item_t *) list->list[0])->key;

	for (i = 1; i < list->count; i++) {
		this = ((index_item_t *) list->list[i])->key;

		if (prev >= this) {
			printListIndexItem(list);
			assert(!_("Error"));
		}

		prev = this;
	}
}
#endif /* DEBUG_INDEX */

static void index_item_destroy(index_item_t *p)
{
	free(p);
}

list_t *index_new()
{
	return list_new();
}

void index_add(list_t *list, int key, void *data)
{
#ifdef DEBUG_INDEX
	int count = 0;
#endif /* DEBUG_INDEX */

	index_item_t *item;
	index_item_t *this;
	int min, max, point;
	int len;

	item = index_item_new(key, data);
	len = list->count;

	min = 0;
	max = len - 1;

	for (;;) {
		point = min + (max - min) / 2;

#ifdef DEBUG_INDEX
		if (++count == len * 5) {
			printf(_("[Error] Cyclic error\n"));
			printIndexItem(item);
			printListIndexItem(list);
			assert(0);
		}
#endif /* DEBUG_INDEX */

		if (max < 0) {
			list_ins(list, 0, item);
#ifdef DEBUG_INDEX
			checkList(list);
#endif /* DEBUG_INDEX */
			return;
		}

		if (min >= len) {
			list_add(list, item);
#ifdef DEBUG_INDEX
			checkList(list);
#endif /* DEBUG_INDEX */
			return;
		}

		this = (index_item_t *) list->list[point];

/*
		printf("min = %d max = %d point = %d len = %d offset = %d\n",
			min, max, point, len, offset);
*/

		if (min > max) {
			list_ins(list, point, item);
#ifdef DEBUG_INDEX
			checkList(list);
#endif /* DEBUG_INDEX */
			return;
		}

		if (item->key > this->key) {
			min = point + 1;
			continue;
		}

		if (item->key < this->key) {
			max = point - 1;
			continue;
		}
	}
}

static int getOffsetFromIndex(list_t *list, int key)
{
	index_item_t *this;
	int min, max, point, len;

	len = list->count;

	if (len == 0) {
		return -1;
	}

	min = 0;
	max = len - 1;

	for (;;) {
		point = min + (max - min) / 2;

		if (max < 0 || point >= len || max < min) {
			return -1;
		}

		this = (index_item_t *) list->list[point];

/*
		printf("min = %d max = %d point = %d len = %d offset = %d\n",
			min, max, point, len, offset);
*/

		if (key == this->key) {
			return point;
		}

		if (key > this->key) {
			min = point + 1;
			continue;
		}

		if (key < this->key) {
			max = point - 1;
			continue;
		}
	}
}

void *index_get(list_t *list, int key)
{
	int offset;

	offset = getOffsetFromIndex(list, key);

	if (offset != -1) {
		index_item_t *this;

		this = list->list[offset];
		return this->data;
	}

	return NULL;
}

void index_del(list_t *list, int key)
{
	int offset;

	offset = getOffsetFromIndex(list, key);

	if (offset != -1) {
		list_del_item(list, offset, index_item_destroy);
	}
}

void index_del_with_object(list_t *list, int key, void *f)
{
	int offset;

	offset = getOffsetFromIndex(list, key);

	if (offset != -1) {
		index_item_t *this;
		void (*fce) (void *p);

		this = list->list[offset];

		fce = f;
		fce(this);

		list_del_item(list, offset, index_item_destroy);
	}
}

void index_action(list_t *list, void *f)
{
	int i;

	for (i = 0; i < list->count; i++) {
		index_item_t *this;
		void (*fce) (void *p);

		this = list->list[i];

		fce = f;
		fce(this);
	}
}

void index_destroy(list_t *list)
{
	list_destroy_item(list, index_item_destroy);
}

void index_destroyWithObject(list_t *list, void *f)
{
	index_action(list, f);
	list_destroy_item(list, index_item_destroy);
}
