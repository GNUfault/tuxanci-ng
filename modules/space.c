
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "main.h"
#include "list.h"
#include "space.h"

#define DEBUG_SPACE
//#define ERROR_SUPPORT

#ifdef DEBUG_SPACE


typedef struct recv_struct
{
	int id;
	int x, y;
	int w, h;
} recv_t;

recv_t* newRecv(int id, int x , int y, int w, int h)
{
	recv_t *new;

	new = malloc( sizeof(recv_t) );
	memset(new, 0, sizeof(recv_t));

	new->id = id;
	new->x = x;
	new->y = y;
	new->w = w;
	new->h = h;

	return new;
}

void getStatus(void *p, int *id, int *x, int *y, int *w, int *h)
{
	recv_t *recv;

	recv = p;

	*id = recv->id;
	*x = recv->x;
	*y = recv->y;
	*w = recv->w;
	*h = recv->h;
}

void setStatus(void *p, int x, int y, int w, int h)
{
	recv_t *recv;

	recv = p;

	recv->x = x;
	recv->y = y;
	recv->w = w;
	recv->h = h;
	recv->x = x;
}

void destroyRecv(recv_t *p)
{
	free(p);
}

#endif

static int my_conflictSpace(int x1,int y1,int w1,int h1,int x2,int y2,int w2,int h2)
{
	return (x1<x2+w2 && x2<x1+w1 && y1<y2+h2 && y2<y1+h1);
}

space_t *newSpace(int w, int h, int segW, int segH,
		  void (*getStatus)(void *p, int *id, int *x, int *y, int *w, int *h),
		  void (*setStatus)(void *p, int x, int y, int w, int h))
{
	space_t *new;
	int i, j;

	new = malloc( sizeof(space_t) );
	memset(new, 0, sizeof(space_t));

	new->w = w / segW + 1;
	new->h = h / segH + 1;
	new->segW = segW;
	new->segH = segH;
	new->getStatus = getStatus;
	new->setStatus = setStatus;
	new->list = newList();

	new->area = malloc( new->w * sizeof(list_t **) );

	for( i = 0 ; i < new->w ; i++ )
	{
		new->area[i] = malloc( new->h * sizeof(list_t *) );
	}

	for( i = 0 ; i < new->h ; i++ )
	{
		for( j = 0 ; j < new->w ; j++ )
		{
			new->area[j][i] = newList();
		}
	}

	return new;
}

static void getSegment(space_t *p, int x, int y, int w, int h,
		       int *segX, int *segY, int *segW, int *segH)
{
	*segX = x / p->segW;
	*segY = y / p->segH;
	*segW = ( (x+w) / p->segW + 1 ) - *segX;
	*segH = ( (y+h) / p->segH + 1 ) - *segY;
}

#ifdef ERROR_SUPPORT

void printListID(space_t *space)
{
	int i;

	for( i = 0 ; i < space->list->count ; i++ )
	{
		int id, x, y, w, h;

		space->getStatus(space->list->list[i], &id, &x, &y, &w, &h);
		printf("%d\n", id);
	}

	putchar('\n');
}

static void checkList(space_t *space)
{
	int id, x, y, w, h;
	int thisId;
	int i;

	if( space->list->count == 0 )
	{
		printf("nothing\n");
		return;	
	}

	space->getStatus(space->list->list[0], &thisId, &x, &y, &w, &h);

	for( i = 1 ; i < space->list->count ; i++ )
	{
		space->getStatus(space->list->list[i], &id, &x, &y, &w, &h);

		if( id <= thisId )
		{
			printListID(space);
			assert( ! "error" );
		}

		thisId = id;
	}
}

static void addToList(space_t *space, void *p)
{
	int id, point_id, inc_point_id;
	int x, y, w, h;
	int len;
	int min, max;
	int point;


	len = space->list->count;

	space->getStatus(p, &id, &x, &y, &w, &h);
/*
	printf("addToList (%d)\n", id);
	printListID(space);
	assert( id >= 0 );
*/

	if( len == 0 )
	{
		//printf("OK first\n");
		addList(space->list, p);
		return;
	}

	if( len == 1 )
	{
		space->getStatus(space->list->list[0], &point_id, &x, &y, &w, &h);
		
		if( id > point_id )
		{
			addList(space->list, p);
		}

		if( id < point_id )
		{
			insList(space->list, 0, p);
		}

		//printf("OK\n");

		return;
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
		printf("min = %d max = %d point = %d len = %d id = %d\n", min, max, point, len, id);

		if(  max < 0 )
		{
			//printf("OK first\n");
			insList(space->list, 0, p);
			checkList(space);
			return;
		}

		if(  point+1 >= len /*|| min >= len*/ )
		{
			addList(space->list, p);
			//printf("OK height\n");
			checkList(space);
			return;
		}
		
		space->getStatus(space->list->list[point], &point_id, &x, &y, &w, &h);
		space->getStatus(space->list->list[point+1], &inc_point_id, &x, &y, &w, &h);

		if( min == max )
		{
			//printf("OK\n");
			insList(space->list, min, p);
			checkList(space);
			return;
		}

		if( point_id < id && id < inc_point_id )
		{
			//printf("OK\n");
			insList(space->list, point+1, p);
			checkList(space);
			return;
		}

		if( id > inc_point_id )
		{
			min = point + 1;
			continue;
		}

		if( id < point_id )
		{
			max = point - 1;
			continue;
		}
	}
}

#endif

void addObjectToSpace(space_t *p, void *item)
{
	int segX, segY, segW, segH;
	int id, x, y, w, h;
	int i, j;

	p->getStatus(item, &id, &x, &y, &w, &h);
	getSegment(p, x, y, w, h, &segX, &segY, &segW, &segH);

	for( i = segY ; i < segY + segH ; i++ )
	{
		for( j = segX ; j < segX + segW ; j++ )
		{
			addList(p->area[j][i], item);
		}
	}

#ifdef ERROR_SUPPORT
	addToList(p, item);
#endif

#ifndef ERROR_SUPPORT
	addList(p->list, item);
#endif
}

void getObjectFromSpace(space_t *p, int x, int y, int w, int h, list_t *list)
{
	int segX, segY, segW, segH;
	int id, this_x, this_y, this_w, this_h;
	int i, j, k;

	getSegment(p, x, y, w, h, &segX, &segY, &segW, &segH);

	for( i = segY ; i < segY + segH ; i++ )
	{
		for( j = segX ; j < segX + segW ; j++ )
		{
			void *this;

			for( k = 0 ; k < p->area[j][i]->count ; k++ )
			{
				this = p->area[j][i]->list[k];
				p->getStatus(this, &id, &this_x, &this_y, &this_w, &this_h);

				if( my_conflictSpace(x, y, w, h, this_x, this_y, this_w, this_h) &&
				     searchListItem(list, this) == -1 )
				{
					addList(list, this);
				}
			}
		}
	}
}

void* getObjectFromSpaceWithID(space_t *space, int id)
{
#ifdef ERROR_SUPPORT
	int point_id;
	int x, y, w, h;
	int len;
	int min, max;
	int point;

	printf("getObjectFromSpaceWithID %d\n", id);
	printListID(space);
	
	len = space->list->count;

	min = 0;
	max = len-1;

	for(;;)
	{
		point = min + ( max - min ) / 2;

		//printf("min = %d max = %d point = %d len = %d id = %d\n", min, max, point, len, id);

		if(  max < 0 || point >= len || max < min )
		{
			return NULL;
		}

		space->getStatus(space->list->list[point], &point_id, &x, &y, &w, &h);

		if( min == max )
		{
			space->getStatus(space->list->list[min], &point_id, &x, &y, &w, &h);
		}

		if( point_id == id )
		{
			return space->list->list[point];
		}

		if( id > point_id )
		{
			min = point + 1;
			continue;
		}

		if( id < point_id )
		{
			max = point - 1;
			continue;
		}
	}

#endif

#ifndef ERROR_SUPPORT
	int this_id, x, y, w, h;
	void *this;
	int i;

	for( i = 0 ; i < space->list->count ; i++ )
	{
		this  = space->list->list[i];
		assert( this != NULL );

		space->getStatus(this, &this_id, &x, &y, &w, &h);

		if( this_id == id )
		{
			return this;
		}
	}

	return NULL;
#endif
}

int isConflictWithObjectFromSpace(space_t *p, int x, int y, int w, int h)
{
	int segX, segY, segW, segH;
	int id, this_x, this_y, this_w, this_h;
	int i, j, k;

	getSegment(p, x, y, w, h, &segX, &segY, &segW, &segH);

	for( i = segY ; i < segY + segH ; i++ )
	{
		for( j = segX ; j < segX + segW ; j++ )
		{
			void *this;

			for( k = 0 ; k < p->area[j][i]->count ; k++ )
			{
				this = p->area[j][i]->list[k];
				p->getStatus(this, &id, &this_x, &this_y, &this_w, &this_h);

				if( my_conflictSpace(x, y, w, h, this_x, this_y, this_w, this_h) )
				{
					return 1;
				}
			}
		}
	}

	return 0;
}

int isConflictWithObjectFromSpaceBut(space_t *p, int x, int y, int w, int h, void *but)
{
	int segX, segY, segW, segH;
	int id, this_x, this_y, this_w, this_h;
	int i, j, k;

	getSegment(p, x, y, w, h, &segX, &segY, &segW, &segH);

	for( i = segY ; i < segY + segH ; i++ )
	{
		for( j = segX ; j < segX + segW ; j++ )
		{
			void *this;

			for( k = 0 ; k < p->area[j][i]->count ; k++ )
			{
				this = p->area[j][i]->list[k];

				if( this == but )
				{
					continue;
				}

				p->getStatus(this, &id, &this_x, &this_y, &this_w, &this_h);

				if( my_conflictSpace(x, y, w, h, this_x, this_y, this_w, this_h) )
				{
					return 1;
				}
			}
		}
	}

	return 0;
}

void delObjectFromSpace(space_t *p, void *item)
{
	int segX, segY, segW, segH;
	int id, x, y, w, h;
	int index;
	int i, j;

	p->getStatus(item, &id, &x, &y, &w, &h);
	getSegment(p, x, y, w, h, &segX, &segY, &segW, &segH);

	for( i = segY ; i < segY + segH ; i++ )
	{
		for( j = segX ; j < segX + segW ; j++ )
		{
			index = searchListItem(p->area[j][i], item);
			assert( index != -1 );
			delList(p->area[j][i], index);
		}
	}

	index = searchListItem(p->list, item);
	assert( index != -1 );
	delList(p->list, index);
}

void delObjectFromSpaceWithObject(space_t *p, void *item, void *f)
{
	void (*fce)(void *param);

	fce = f;

	delObjectFromSpace(p, item);
	fce(item);
}

void moveObjectInSpace(space_t *p, void *item, int move_x, int move_y)
{
	int old_segX, old_segY, old_segW, old_segH;
	int new_segX, new_segY, new_segW, new_segH;
	int id, x, y, w, h;

	p->getStatus(item, &id, &x, &y, &w, &h);
	getSegment(p, x, y, w, h, &old_segX, &old_segY, &old_segW, &old_segH);
	getSegment(p, move_x, move_y, w, h, &new_segX, &new_segY, &new_segW, &new_segH);
/*
	printf("%d %d %d %d  -> %d %d %d %d \n",
		old_segX, old_segY, old_segW, old_segH,
		new_segX, new_segY, new_segW, new_segH);
*/
	if( old_segX != new_segX || old_segY != new_segY ||
	    old_segW != new_segW || old_segH != new_segH )
	{
		delObjectFromSpace(p, item);
		p->setStatus(item, move_x, move_y, w, h);
		addObjectToSpace(p, item);
		return;
	}

	p->setStatus(item, move_x, move_y, w, h);
}

void printSpace(space_t *p)
{
	int i, j;

	printf("print space : \n");

	for( i = 0 ; i < p->h ; i++ )
	{
		for( j = 0 ; j < p->w ; j++ )
		{
			printf("%3d ", p->area[j][i]->count);
		}

		putchar('\n');
	}
}

void destroySpace(space_t *p)
{
	int j, i;

	destroyList(p->list);

	for( i = 0 ; i < p->h ; i++ )
	{
		for( j = 0 ; j < p->w ; j++ )
		{
			destroyList(p->area[j][i]);
		}
	}

	for( i = 0 ; i < p->w ; i++ )
	{
		free(p->area[i]);
	}

	free(p->area);
	free(p);
}

void destroySpaceWithObject(space_t *p, void *f)
{
	void (*fce)(void *param);
	int i;

	fce = f;

	for( i = 0 ; i < p->list->count ; i++ )
		fce(p->list->list[i]);

	destroySpace(p);
}

#ifdef DEBUG_SPACE

void test_space()
{
	space_t *space;
	recv_t *recv;

	space = newSpace(5000, 2500, 320, 240, getStatus, setStatus);

	addObjectToSpace(space, newRecv(6, 0, 0, 1, 1) );
	addObjectToSpace(space, newRecv(3, 0, 0, 1, 1) );
	addObjectToSpace(space, newRecv(4, 0, 0, 1, 1) );

	//printListID(space);
/*
	recv = getObjectFromSpaceWithID(space, 6);
	if( recv != NULL )printf("id = %d\n", recv->id);
*/
	recv = getObjectFromSpaceWithID(space, 3);

	if( recv != NULL )
	{
		printf("id = %d\n", recv->id);
	}
	else
	{
		printf("recv = %p\n", recv);
	}

/*
	recv = getObjectFromSpaceWithID(space, 4);
	if( recv != NULL )printf("id = %d\n", recv->id);
	recv = getObjectFromSpaceWithID(space, 1);
	if( recv != NULL )printf("id = %d\n", recv->id);
*/

#if 0
	for( i = 1 ; i < 16 ; i++ )
		for( j = 1 ; j < 16 ; j++ )
		{
			recv_t *this;

			this = newRecv(j*160, i*120, 32, 32);
			//addToSpace(space, newRecv(j*550, i*550, 1, 1) );
			addObjectToSpace(space, this );
			delObjectFromSpaceWithMem(space, this, destroyRecv);
		}
#endif
/*
	list = newList();
	getObjectFromSpace(space, 640, 480, 800 , 600, list);

	printf("list->count = %d\n", list->count);

	for( i = 0 ; i < list->count ; i++ )
	{
		recv_t *recv;
		recv = list->list[i];

		printf("%3d %3d %3d %3d\n", recv->x, recv->y, recv->w, recv->h);
	}
*/
/*
	destroyList(list);

	printSpace(space);

	destroySpace(space);
*/
}

#endif

