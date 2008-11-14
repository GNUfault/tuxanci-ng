
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "main.h"
#include "list.h"

typedef struct struct_storage_item {
	char *name;
	char *group;
	void *data;
} storage_item_t;

list_t *newStorage()
{
	return newList();
}

static storage_item_t *newStorageItem(char *group, char *name, void *data)
{
	storage_item_t *new;

	assert(name != NULL);
	assert(group != NULL);
	assert(data != NULL);
	new = malloc(sizeof(storage_item_t));
	new->name = strdup(name);
	new->group = strdup(group);
	new->data = data;

	return new;
}

static void destroyStorageItem(storage_item_t * p, void *f)
{
	void (*fce) (void *);

	assert(p != NULL);
	assert(f != NULL);

	fce = f;
	fce(p->data);
	free(p->name);
	free(p->group);
	free(p);
}

void addItemToStorage(list_t * list, char *group, char *name, void *data)
{
	assert(list != NULL);
	assert(group != NULL);
	assert(name != NULL);

	addList(list, newStorageItem(group, name, data));
}

void *getItemFromStorage(list_t * list, char *group, char *name)
{
	storage_item_t *this;
	int i;

	assert(list != NULL);
	assert(group != NULL);
	assert(name != NULL);

	for (i = 0; i < list->count; i++) {
		this = (storage_item_t *) list->list[i];

		if (strcmp(group, this->group) == 0 && strcmp(name, this->name) == 0)
			return this->data;
	}

	DEBUG_MSG(_("%s %s was not found in storage!\n"), group, name);

	return NULL;
}

void delItemFromStorage(list_t * list, char *group, char *name, void *f)
{
	storage_item_t *this;
	int i;

	assert(list != NULL);
	assert(group != NULL);
	assert(name != NULL);

	for (i = 0; i < list->count; i++) {
		this = (storage_item_t *) list->list[i];

		if (strcmp(group, this->group) == 0 && strcmp(name, this->name) == 0) {
			destroyStorageItem(this, f);
			delList(list, i);
			return;
		}
	}

	return;
}

void delAllItemFromStorage(list_t * list, char *group, void *f)
{
	storage_item_t *this;
	int i;

	assert(list != NULL);
	assert(group != NULL);

	for (i = 0; i < list->count; i++) {
		this = (storage_item_t *) list->list[i];

		if (strcmp(group, this->group) == 0) {
			destroyStorageItem(this, f);
			delList(list, i);
			i--;
		}
	}
}

void destroyStorage(list_t * p, void *f)
{
	storage_item_t *this;
	int i;

	assert(p != NULL);
	assert(f != NULL);

	for (i = 0; i < p->count; i++) {
		this = (storage_item_t *) p->list[i];

		destroyStorageItem(this, f);
	}

	destroyList(p);
}
