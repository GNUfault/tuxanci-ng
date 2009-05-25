#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "main.h"
#include "list.h"
#include "space.h"
#include "index.h"

zone_t *newZone()
{
	zone_t *new;

	new = malloc(sizeof(zone_t));
	new->list = list_new();

	return new;
}

void destroyZone(zone_t *p)
{
	assert(p != NULL);

	list_destroy(p->list);
	free(p);
}

static int my_arena_conflict_space(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2)
{
	return (x1 < x2 + w2 && x2 < x1 + w1 && y1 < y2 + h2 && y2 < y1 + h1);
}

space_t *space_new(int w, int h, int segW, int segH,
		   void (*getStatus) (void *p, int *id, int *x, int *y, int *w, int *h),
		   void (*setStatus) (void *p, int x, int y, int w, int h))
{
	space_t *new;
	int i, j;

	new = malloc(sizeof(space_t));
	memset(new, 0, sizeof(space_t));

	new->w = w / segW + 1;
	new->h = h / segH + 1;
	new->segW = segW;
	new->segH = segH;
	new->getStatus = getStatus;
	new->setStatus = setStatus;
	new->listIndex = index_new();

	new->zone = malloc(new->w * sizeof(list_t **));

	for (i = 0; i < new->w; i++) {
		new->zone[i] = malloc(new->h * sizeof(list_t *));
	}

	for (i = 0; i < new->h; i++) {
		for (j = 0; j < new->w; j++) {
			new->zone[j][i] = newZone();
		}
	}

	return new;
}

static void getSegment(space_t *p, int x, int y, int w, int h,
				   int *segX, int *segY, int *segW, int *segH)
{
	*segX = x / p->segW;
	*segY = y / p->segH;
	*segW = ((x + w) / p->segW + 1) - *segX;
	*segH = ((y + h) / p->segH + 1) - *segY;
}

int space_get_count(space_t *p)
{
	return p->listIndex->count;
}

void *space_get_item(space_t *p, int offset)
{
	index_item_t *this;

	this = (index_item_t *) p->listIndex->list[offset];
	return this->data;
}

void space_add(space_t *p, void *item)
{
	int segX, segY, segW, segH;
	int id, x, y, w, h;
	int i, j;

	p->getStatus(item, &id, &x, &y, &w, &h);
	getSegment(p, x, y, w, h, &segX, &segY, &segW, &segH);

	for (i = segY; i < segY + segH; i++) {
		for (j = segX; j < segX + segW; j++) {
			if (j < 0 || j >= p->w || i < 0 || i >= p->h) {
				continue;
			}

			list_add(p->zone[j][i]->list, item);
		}
	}

	index_add(p->listIndex, id, item);
}

void space_get_object(space_t *p, int x, int y, int w, int h, list_t *list)
{
	int segX, segY, segW, segH;
	int id, this_x, this_y, this_w, this_h;
	int i, j, k;

	getSegment(p, x, y, w, h, &segX, &segY, &segW, &segH);

	for (i = segY; i < segY + segH; i++) {
		for (j = segX; j < segX + segW; j++) {
			void *this;

			if (j < 0 || j >= p->w || i < 0 || i >= p->h) {
				continue;
			}

			for (k = 0; k < p->zone[j][i]->list->count; k++) {
				this = p->zone[j][i]->list->list[k];
				p->getStatus(this, &id, &this_x, &this_y, &this_w, &this_h);

				if (my_arena_conflict_space(x, y, w, h, this_x, this_y, this_w, this_h) &&
				    list_search(list, this) == -1) {
					list_add(list, this);
				}
			}
		}
	}
}

void *space_get_object_id(space_t *space, int id)
{
	return index_get(space->listIndex, id);
}

int space_is_conflict_with_object(space_t *p, int x, int y, int w, int h)
{
	int segX, segY, segW, segH;
	int id, this_x, this_y, this_w, this_h;
	int i, j, k;

	getSegment(p, x, y, w, h, &segX, &segY, &segW, &segH);

	for (i = segY; i < segY + segH; i++) {
		for (j = segX; j < segX + segW; j++) {
			void *this;

			if (j < 0 || j >= p->w || i < 0 || i >= p->h) {
				continue;
			}

			for (k = 0; k < p->zone[j][i]->list->count; k++) {
				this = p->zone[j][i]->list->list[k];
				p->getStatus(this, &id, &this_x, &this_y, &this_w, &this_h);

				if (my_arena_conflict_space(x, y, w, h, this_x, this_y, this_w, this_h)) {
					return 1;
				}
			}
		}
	}

	return 0;
}

int space_is_conflict_with_object_but(space_t *p, int x, int y, int w, int h, void *but)
{
	int segX, segY, segW, segH;
	int id, this_x, this_y, this_w, this_h;
	int i, j, k;

	getSegment(p, x, y, w, h, &segX, &segY, &segW, &segH);

	for (i = segY; i < segY + segH; i++) {
		for (j = segX; j < segX + segW; j++) {
			void *this;

			if (j < 0 || j >= p->w || i < 0 || i >= p->h) {
				continue;
			}

			for (k = 0; k < p->zone[j][i]->list->count; k++) {
				this = p->zone[j][i]->list->list[k];

				if (this == but) {
					continue;
				}

				p->getStatus(this, &id, &this_x, &this_y, &this_w, &this_h);

				if (my_arena_conflict_space(x, y, w, h, this_x, this_y, this_w, this_h)) {
					return 1;
				}
			}
		}
	}

	return 0;
}

void space_del(space_t *p, void *item)
{
	int segX, segY, segW, segH;
	int id, x, y, w, h;
	int myIndex;
	int i, j;

	p->getStatus(item, &id, &x, &y, &w, &h);
	getSegment(p, x, y, w, h, &segX, &segY, &segW, &segH);

	for (i = segY; i < segY + segH; i++) {
		for (j = segX; j < segX + segW; j++) {
			if (j < 0 || j >= p->w || i < 0 || i >= p->h) {
				continue;
			}

			myIndex = list_search(p->zone[j][i]->list, item);
			assert(myIndex != -1);
			list_del(p->zone[j][i]->list, myIndex);
		}
	}

	index_del(p->listIndex, id);
}

void space_del_with_item(space_t *p, void *item, void *f)
{
	void (*fce) (void *param);

	fce = f;

	space_del(p, item);
	fce(item);
}

void space_move_object(space_t *p, void *item, int move_x, int move_y)
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

	if (old_segX != new_segX || old_segY != new_segY ||
	    old_segW != new_segW || old_segH != new_segH) {
		space_del(p, item);
		p->setStatus(item, move_x, move_y, w, h);
		space_add(p, item);
		return;
	}

	p->setStatus(item, move_x, move_y, w, h);
}

void space_print(space_t *p)
{
	int i, j;

	printf(_("[Debug] Debugging space:\n"));

	for (i = 0; i < p->h; i++) {
		for (j = 0; j < p->w; j++) {
			printf("%3d ", p->zone[j][i]->list->count);
		}

		putchar('\n');
	}
}

void space_action(space_t *space, void *f, void *p)
{
	void (*fce) (space_t *space, void *f, void *p);
	int len;
	int i;

	fce = f;
	len = space->listIndex->count;

	for (i = 0; i < len; i++) {
		fce(space, space_get_item(space, i), p);

		if (space->listIndex->count == len - 1) {
			len--;
			i--;
		}
	}
}

void space_action_from_location(space_t *space, void *f, void *p, int x, int y, int w, int h)
{
	void (*fce) (space_t *space, void *f, void *p);
	list_t *list;
	int len;
	int i;

	fce = f;

	list = list_new();

	space_get_object(space, x, y, w, h, list);

	len = list->count;

	for (i = 0; i < len; i++) {
		fce(space, list->list[i], p);
	}

	list_destroy(list);
}

void space_destroy(space_t *p)
{
	int j, i;

	index_destroy(p->listIndex);

	for (i = 0; i < p->h; i++) {
		for (j = 0; j < p->w; j++) {
			destroyZone(p->zone[j][i]);
		}
	}

	for (i = 0; i < p->w; i++) {
		free(p->zone[i]);
	}

	free(p->zone);
	free(p);
}

void space_destroy_with_item(space_t *p, void *f)
{
	void (*fce) (void *param);
	int i;

	fce = f;

	for (i = 0; i < p->listIndex->count; i++) {
		fce(space_get_item(p, i));
	}

	space_destroy(p);
}
