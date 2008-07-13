
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "main.h"
#include "list.h"
#include "idManager.h"

static list_t *listID;
static int lastID;

typedef struct id_item_struct
{
	int id;
	int count;
} id_item_t;

static id_item_t* newIdItem(int id, int count)
{
	id_item_t *new;

	new = malloc( sizeof(id_item_t) );
	new->id = id;
	new->count = count;

	return new;
}

static void destroyIdItem(id_item_t *p)
{
	assert( p != NULL );

	free(p);
}

void initListID()
{
	listID = newList();
	lastID = 0;
	printf("init ID manger..\n");
}

int isRegisterID(int id)
{
	int i;

	assert( listID != NULL );

	for( i = 0 ; i < listID->count ; i++ )
	{
		id_item_t *this;

		this = listID->list[i];

		if( this->id == id )
		{
			return i;
		}
	}

	return -1;
}

static int findNewID()
{
	int ret;

	assert( listID != NULL );

	do{
		ret  = random() % MAX_ID;

	}while( isRegisterID(ret) != -1 );

	return ret;
}

int getNewIDcount(int count)
{
	int id;

	id = findNewID();

	addList(listID, newIdItem(id, count) );

	return id;
}

int getNewID()
{
	return getNewIDcount(1);
}

void incID(int id)
{
	id_item_t *this;
	int index;
	
	assert( listID != NULL );

	index = isRegisterID(id);

	if( index == -1 )
	{
		assert( ! "takyto ID nebol nikdy registrovany !" );
		return; // ha ha ha
	}

	this = listID->list[index];

	this->count++;

	//printf("inc ID %d %d\n", this->id, this->count);
	return;
}

void delID(int id)
{
	id_item_t *this;
	int index;
	
	assert( listID != NULL );

	index = isRegisterID(id);

	if( index == -1 )
	{
		assert( ! "takyto ID nebol nikdy registrovany !" );
		return; // ha ha ha
	}

	this = listID->list[index];

	this->count--;
	//printf("dec ID %d %d\n", this->id, this->count);

	if( this->count <= 0 )
	{
		delListItem(listID, index, free);
	}
	
	return;
}

void replaceID(int old_id, int new_id)
{
	int index_old_id;
	int index_new_id;
	id_item_t *this;

	if( old_id == new_id )
	{
		return;
	}

	index_old_id = isRegisterID(old_id);
	assert( index_old_id != -1 );

	index_new_id = isRegisterID(new_id);
	assert( index_new_id == -1 );

	this = listID->list[index_old_id];
	this->id = new_id;
}

void infoID(int id)
{
	id_item_t *this;
	int index;
	
	assert( listID != NULL );

	index = isRegisterID(id);

	if( index == -1 )
	{
		printf("ID %d not exists\n", id);
		return;
	}

	this = listID->list[index];

	printf("ID %d ( count %d )\n", this->id, this->count);

	return;
}

void quitListID()
{
	printf("quit ID manger..\n");

	assert( listID != NULL );
	destroyListItem(listID, destroyIdItem);
}

