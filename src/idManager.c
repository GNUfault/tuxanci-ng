
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "main.h"
#include "list.h"
#include "idManager.h"

static list_t *listID;
static int lastID;

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
		int *z;

		z = (int *) listID->list[i];

		if( *z == id )
		{
			return i;
		}
	}

	return -1;
}

int getNewID()
{
	int ret;

	assert( listID != NULL );

	do{
		ret  = random() % MAX_ID;
/*
		ret = ++lastID;

		if( lastID > MAX_ID )
		{
			lastID = 0;
		}
*/
	}while( isRegisterID(ret) != -1 );

	addList(listID, newInt(ret) );

	//printf("new ID -> %d\n", ret);
	return ret;
}

void delID(int id)
{
	int index;

	assert( listID != NULL );

	index = isRegisterID(id);

	if( index == -1 )
	{
		assert( ! "takyto ID nebol nikdy registrovany !" );
		return; // ha ha ha
	}

	delListItem(listID, index, free);

	return;
}

void replaceID(int old_id, int new_id)
{
	if( old_id == new_id )
	{
		return;
	}

	delID(old_id);

	if( new_id != -1 )
	{
		assert( isRegisterID(new_id) == -1 );
		addList(listID, newInt(new_id) );
	}
}

void quitListID()
{
	printf("quit ID manger..\n");

	assert( listID != NULL );
	destroyListItem(listID, free);
}

