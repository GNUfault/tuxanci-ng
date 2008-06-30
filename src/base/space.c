
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "main.h"
#include "list.h"
#include "space.h"
#include "index.h"

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
	new->listIndex = newIndex();

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

void addObjectToSpace(space_t *p, void *item)
{
	int segX, segY, segW, segH;
	int id, x, y, w, h;
	int i, j;
	int offset;

	p->getStatus(item, &id, &x, &y, &w, &h);
	getSegment(p, x, y, w, h, &segX, &segY, &segW, &segH);

	for( i = segY ; i < segY + segH ; i++ )
	{
		for( j = segX ; j < segX + segW ; j++ )
		{
			if( j < 0 || j >= p->w || i < 0 || i >= p->h )
			{
				continue;
			}

			addList(p->area[j][i], item);
		}
	}

	offset = addToIndex(p->listIndex, id);
	insList(p->list, offset, item);
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
				if( j < 0 || j >= p->w || i < 0 || i >= p->h )
				{
					continue;
				}

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
	int index;

	index = getFormIndex(space->listIndex, id);

	return ( index != -1 ? space->list->list[index] : NULL );
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
				if( j < 0 || j >= p->w || i < 0 || i >= p->h )
				{
					continue;
				}

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
				if( j < 0 || j >= p->w || i < 0 || i >= p->h )
				{
					continue;
				}

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
			if( j < 0 || j >= p->w || i < 0 || i >= p->h )
			{
				continue;
			}

			index = searchListItem(p->area[j][i], item);
			assert( index != -1 );
			delList(p->area[j][i], index);
		}
	}

	index = getFormIndex(p->listIndex, id);
	assert( index != -1 );

	delList(p->list, index);

	delFromIndex(p->listIndex, id);
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
	destroyIndex(p->listIndex);

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
{/*
	space_t *space;
	recv_t *recv;

	space = newSpace(5000, 2500, 320, 240, getStatus, setStatus);

	addObjectToSpace(space, newRecv(5, 0, 0, 1, 1) );
	addObjectToSpace(space, newRecv(7, 0, 0, 1, 1) );
	addObjectToSpace(space, newRecv(8, 0, 0, 1, 1) );
	addObjectToSpace(space, newRecv(9, 0, 0, 1, 1) );
	addObjectToSpace(space, newRecv(6, 0, 0, 1, 1) );
*/
/*
	recv = getObjectFromSpaceWithID(space, 6);
	if( recv != NULL )printf("id = %d\n", recv->id);
*/
//	printListID(space);
}

#endif

