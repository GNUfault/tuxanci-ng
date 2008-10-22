
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "main.h"
#include "list.h"
#include "index.h"

#define DEBUG_INDEX

static index_item_t* newIndexItem(int key, void *data)
{
	index_item_t *new;

	new = malloc( sizeof(index_item_t) );
	new->key = key;
	new->data = data;

	return new;
}

#ifdef DEBUG_INDEX
static void printIndexItem(index_item_t *p)
{
	printf("key = %d data = %p\n", p->key, p->data);
}

static void printListIndexItem(list_t *list)
{
	int i;

	printf("list :\n");
	printf("------------------\n");

	for( i = 0 ; i < list->count ; i++ )
	{
		index_item_t *this;

		this = (index_item_t *)list->list[i];
		printIndexItem(this);
	}
}

static void checkList(list_t *list)
{
	int i;
	int prev;
	int this;

	//return;

	if( list->count == 0 )
	{
		printf("nothing\n");
		return;	
	}

	prev = ( (index_item_t *) list->list[0] )->key;

	for( i = 1 ; i < list->count ; i++ )
	{
		this = ( (index_item_t *) list->list[i] )->key;

		if( prev >= this )
		{
			printListIndexItem(list);
			assert( ! "error" );
		}

		prev = this;
	}
}
#endif

static void destroyIndexItem(index_item_t *p)
{
	free(p);
}

list_t* newIndex()
{
	return newList();
}

void addToIndex(list_t *list, int key, void *data)
{
#ifdef DEBUG_INDEX
	int count = 0;
#endif

	index_item_t *item;
	index_item_t *this;
	int min, max, point;
	int len;

	item = newIndexItem(key, data);
	len = list->count;

	min = 0;
	max = len-1;

	for(;;)
	{
		point = min + ( max - min ) / 2;

#ifdef DEBUG_INDEX
		if( ++count == len*5 )
		{
			printf("CICLIC ERROR\n");
			printIndexItem(item);
			printf("-------------------\n");
			printListIndexItem(list);
			assert( 0 );
		}
#endif

		if( max < 0 )
		{
			insList(list, 0, item);
#ifdef DEBUG_INDEX
			checkList(list);
#endif
			return;
		}

		if( min >= len )
		{
			addList(list, item);
#ifdef DEBUG_INDEX
			checkList(list);
#endif
			return;
		}
		
		this = (index_item_t *)list->list[point];

/*
		printf("min = %d max = %d point = %d len = %d offset = %d\n",
			min, max, point, len, offset);

*/
		if( min > max )
		{
			insList(list, point, item);
#ifdef DEBUG_INDEX
			checkList(list);
#endif
			return;
		}

		if( item->key > this->key )
		{
			min = point + 1;
			continue;
		}

		if( item->key < this->key )
		{
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

	if( len == 0 )
	{
		return -1;
	}

	min = 0;
	max = len-1;

	for(;;)
	{
		point = min + ( max - min ) / 2;

		if(  max < 0 || point >= len || max < min )
		{
			return -1;
		}
		
		this = (index_item_t *)list->list[point];
/*
		printf("min = %d max = %d point = %d len = %d offset = %d\n",
			min, max, point, len, offset);
*/
		if( key == this->key )
		{
			return point;
		}

		if( key > this->key )
		{
			min = point + 1;
			continue;
		}

		if( key < this->key )
		{
			max = point - 1;
			continue;
		}
	}
}

void* getFromIndex(list_t *list, int key)
{
	int offset;

	offset = getOffsetFromIndex(list, key);

	if( offset != -1 )
	{
		index_item_t *this;

		this = list->list[offset];
		return this->data;
	}

	return NULL;
}

void delFromIndex(list_t *list, int key)
{
	int offset;

	offset = getOffsetFromIndex(list, key);

	if( offset != -1 )
	{
		delListItem(list, offset, destroyIndexItem);
	}
}

void delFromIndexWithObject(list_t *list, int key, void *f)
{
	int offset;

	offset = getOffsetFromIndex(list, key);

	if( offset != -1 )
	{
		index_item_t *this;
		void (*fce)(void *p);

		this = list->list[offset];

		fce = f;
		fce(this);

		delListItem(list, offset, destroyIndexItem);
	}
}

void actionIndexWithObject(list_t *list, void *f)
{
	int i;

	for( i = 0 ; i < list->count ; i++)
	{
		index_item_t *this;
		void (*fce)(void *p);

		this = list->list[i];

		fce = f;
		fce(this);
	}
}

void destroyIndex(list_t *list)
{
	destroyListItem(list, destroyIndexItem);
}

void destroyIndexWithObject(list_t *list, void *f)
{
	actionIndexWithObject(list, f);
	destroyListItem(list, destroyIndexItem);
}
