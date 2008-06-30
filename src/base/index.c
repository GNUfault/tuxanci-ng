
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "main.h"
#include "list.h"
#include "index.h"

static int* index_newInt(int x)
{
	int *new;
	new = malloc( sizeof(int) );
	*new = x;
	return new;
}

static void printIndex(list_t *list)
{
	int i;

	printf("printIndex\n");

	for( i = 0 ; i < list->count ; i++ )
	{
		int thisIndex;

		thisIndex = *(int *)list->list[i];
		printf("%d\n", thisIndex);
	}
}

static void checkIndex(list_t *list)
{
	int index, thisIndex;
	int i;

	if( list->count == 0 )
	{
		printf("nothing\n");
		return;	
	}

	thisIndex = *(int *)list->list[0];

	for( i = 1 ; i < list->count ; i++ )
	{
		index = *(int *)list->list[i];

		if( index <= thisIndex )
		{
			printIndex(list);
			assert( ! "error" );
		}

		thisIndex = index;
	}
}

list_t* newIndex()
{
	return newList();
}

int addToIndex(list_t *list, int index)
{
	int point_index, inc_point_index;
	int len;
	int min, max;
	int point;

	len = list->count;

	//printf("addToIndex (%p, %d)\n", list, index);
	//printIndex(list);

	if( len == 0 )
	{
		//printf("OK first\n");
		addList(list, index_newInt(index));
		checkIndex(list);
		return 0;
	}

	if( len == 1 )
	{
		//space->getStatus(space->list->list[0], &point_id, &x, &y, &w, &h);
		point_index = *(int *)list->list[0];
		
		if( index > point_index )
		{
			addList(list, index_newInt(index));
			//checkIndex(list);
			return 1;
		}

		if( index < point_index )
		{
			insList(list, 0, index_newInt(index));
			//checkIndex(list);
			return 0;
		}

		//printf("OK\n")
		return -1;
	}

	min = 0;
	max = len-1;
/*
	if( max < 0 )
	{
		max = 0;
	}
*/
	for(;;)
	{
		point = min + ( max - min ) / 2;
/*
		space->getStatus(space->list->list[min], &id_min, &x, &y, &w, &h);
		space->getStatus(space->list->list[max], &id_max, &x, &y, &w, &h);
*/
		//printf("min = %d max = %d point = %d len = %d id = %d\n", min, max, point, len, id);

		if(  max < 0 )
		{
			//printf("OK first\n");
			insList(list, 0, index_newInt(index));
			//checkIndex(list);
			return 0;
		}

		if(  point+1 >= len /*|| min >= len*/ )
		{
			addList(list, index_newInt(index) );
			//printf("OK height\n");
			//checkIndex(list);
			return len;
		}
		
//		space->getStatus(space->list->list[point], &point_id, &x, &y, &w, &h);
//		space->getStatus(space->list->list[point+1], &inc_point_id, &x, &y, &w, &h);

		point_index = *(int *)list->list[point];
		inc_point_index = *(int *)list->list[point+1];
/*
		if( min == max )
		{
			//printf("OK\n");
			insList(space->list, min, p);
			checkList(space);
			return;
		}
*/
		if( point_index < index && index < inc_point_index )
		{
			//printf("OK\n");
			insList(list, point+1,  index_newInt(index) );
			//checkIndex(list);
			return point+1;
		}

		if( index > inc_point_index )
		{
			min = point + 1;
			continue;
		}

		if( index < point_index )
		{
			max = point - 1;
			continue;
		}
	}
}

int getFormIndex(list_t *list, int index)
{
	int point_index;
	int len;
	int min, max;
	int point;


	//printf("getFormIndex %d\n", index);
	//printIndex(list);

	len =list->count;

	min = 0;
	max = len-1;

	for(;;)
	{
		point = min + ( max - min ) / 2;

		//printf("min = %d max = %d point = %d len = %d id = %d\n", min, max, point, len, id);

		if(  max < 0 || point >= len || max < min )
		{
			return -1;
		}

		//space->getStatus(space->list->list[point], &point_id, &x, &y, &w, &h);

		point_index = *(int *)list->list[point];

		if( min == max )
		{
			//space->getStatus(space->list->list[min], &point_id, &x, &y, &w, &h);
			point_index = *(int *)list->list[point];
		}

		if( point_index == index )
		{
			return point;
		}

		if( index > point_index )
		{
			min = point + 1;
			continue;
		}

		if( index < point_index )
		{
			max = point - 1;
			continue;
		}
	}
}

void delFromIndex(list_t *list, int index)
{
	int offset;

	offset = getFormIndex(list, index);
	assert( offset != -1 );

	delListItem(list, offset, free);
}

void destroyIndex(list_t *list)
{
	assert( list != NULL);

	destroyListItem(list, free);
}
