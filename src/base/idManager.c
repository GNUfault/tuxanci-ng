
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "main.h"
#include "list.h"
#include "idManager.h"

static list_t *listID;
static int lastID;

typedef struct id_item_struct {
	int id;
	int count;
} id_item_t;

static id_item_t *newIdItem(int id, int count)
{
	id_item_t *new;

	new = malloc(sizeof(id_item_t));
	new->id = id;
	new->count = count;

	return new;
}

static void destroyIdItem(id_item_t * p)
{
	assert(p != NULL);

	free(p);
}

void id_init_list()
{
	listID = list_new();
	lastID = 0;

	DEBUG_MSG(_("Starting ID manger\n"));

}

int id_is_register(int id)
{
	int i;

	assert(listID != NULL);

	for (i = 0; i < listID->count; i++) {
		id_item_t *this;

		this = listID->list[i];

		if (this->id == id) {
			return i;
		}
	}

	return -1;
}

static int findNewID()
{
	int ret;

	assert(listID != NULL);

	if (listID->count >= MAX_ID - 1) {
		assert(!_("No free ID left!"));
	}

	do {
		//ret  = ( random() % (listID->count + 8 ) ) + 1;
		ret = random() % MAX_ID + 1;
	} while (id_is_register(ret) != -1);

	//printf("new ID %d\n", ret);

	return ret;
}

int id_get_newcount(int count)
{
	int id;

	id = findNewID();

	list_add(listID, newIdItem(id, count));

	return id;
}

int id_get_new()
{
	return id_get_newcount(1);
}

void id_inc(int id)
{
	id_item_t *this;
	int index;

	assert(listID != NULL);

	index = id_is_register(id);

	if (index == -1) {
		assert(!_("This kind of ID was never registered!"));
		return;	// ha ha ha
	}

	this = listID->list[index];

	this->count++;

	//printf("inc ID %d %d\n", this->id, this->count);
	return;
}

void id_del(int id)
{
	id_item_t *this;
	int index;

	assert(listID != NULL);

	index = id_is_register(id);

	if (index == -1) {
		assert(!_("This kind of ID was never registered!"));
		return;	// ha ha ha
	}

	this = listID->list[index];

	this->count--;
	//printf("dec ID %d %d\n", this->id, this->count);

	if (this->count <= 0) {
		list_del_item(listID, index, free);
		//printf("listID->count = %d\n", listID->count);
	}

	return;
}

void id_replace(int old_id, int new_id)
{
	int index_old_id;
	int index_new_id;
	id_item_t *this;

	if (old_id == new_id) {
		return;
	}

	index_old_id = id_is_register(old_id);
	assert(index_old_id != -1);

	index_new_id = id_is_register(new_id);
	assert(index_new_id == -1);

	this = listID->list[index_old_id];
	this->id = new_id;
}

void infoID(int id)
{
	id_item_t *this;
	int index;

	assert(listID != NULL);

	index = id_is_register(id);

	if (index == -1) {
		DEBUG_MSG(_("ID %d does not exist\n"), id);

		return;
	}

	this = listID->list[index];

	DEBUG_MSG(_("ID %d (count %d)\n"), this->id, this->count);

	return;
}

void id_quit_list()
{
	DEBUG_MSG(_("Quitting ID manger\n"));

	assert(listID != NULL);
	list_destroy_item(listID, destroyIdItem);
}
