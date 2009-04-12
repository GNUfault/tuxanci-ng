
#ifndef SPACE_H

#    define SPACE_H

#    include "main.h"
#    include "list.h"

typedef struct zone_struct {
	list_t *list;
} zone_t;

typedef struct space_struct {
	int w;
	int h;

	int segW;
	int segH;

	zone_t ***zone;

	list_t *listIndex;

	void (*getStatus) (void *p, int *id, int *x, int *y, int *w, int *h);
	void (*setStatus) (void *p, int x, int y, int w, int h);
} space_t;

extern space_t *space_new(int w, int h, int segW, int segH,
						 void (*getStatus) (void *p, int *id, int *x, int *y,
											int *w, int *h),
						 void (*setStatus) (void *p, int x, int y, int w,
											int h));

extern int space_get_count(space_t * p);
extern void *space_get_item(space_t * p, int offset);
extern void space_add(space_t * p, void *item);
extern void space_get_object(space_t * p, int x, int y, int w, int h,
							   list_t * list);
extern void *space_get_object_id(space_t * p, int id);
extern int space_is_conflict_with_object(space_t * p, int x, int y, int w,
										 int h);
extern int space_is_conflict_with_object_but(space_t * p, int x, int y, int w,
											int h, void *but);
extern void space_del(space_t * p, void *item);
extern void space_del_with_item(space_t * p, void *item, void *f);
extern void space_move_object(space_t * p, void *item, int move_x, int move_y);
extern void space_print(space_t * p);
extern void space_action(space_t * space, void *f, void *p);
extern void space_action_from_location(space_t * space, void *f, void *p, int x,
									int y, int w, int h);
extern void space_destroy(space_t * p);
extern void space_destroy_with_item(space_t * p, void *f);

#endif
