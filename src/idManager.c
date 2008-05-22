
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "main.h"
#include "list.h"
#include "idManager.h"

list_t * newListID()
{
	return newList();
}

int getNewID(list_t *p)
{
	int ret;
	int i;

	assert( p != NULL );

	do{
		ret = random() % MAX_ID + 1;

		for( i = 0 ; i < p->count ; i++ )
		{
			int *z;

			z = (int *) p->list[i];

			if( *z == ret )continue;
		}

	}while(0);

	addList(p, newInt(ret) );

	//printf("new ID -> %d\n", ret);
	return ret;
}

int isRegisterID(list_t *p, int id)
{
	int i;

	assert( p != NULL );

	for( i = 0 ; i < p->count ; i++ )
	{
		int *z;

		z = (int *) p->list[i];

		if( *z == id )
		{
			return i;
		}
	}

	return -1;
}

void delID(list_t *p, int id)
{
	int index;

	assert( p != NULL );

	index = isRegisterID(p, id);

	if( index == -1 )
	{
		assert( ! "takyto ID nebol nikdy registrovany !" );
		return; // ha ha ha
	}

	delListItem(p, index, free);

	return;
}

void replaceID(list_t *p, int old_id, int new_id)
{
	delID(p, old_id);

	if( new_id != -1 )
	{
		assert( isRegisterID(p, new_id) == -1 );
		addList(p, newInt(new_id) );
	}
}

void destroyListID(list_t *p)
{
	assert( p != NULL );
	destroyListItem(p, free);
}
