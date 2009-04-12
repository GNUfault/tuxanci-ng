
#ifndef ITEM_H
#    define ITEM_H
#    define ITEM_SYNC_TIMEOUT			5000
#    ifdef __WIN32__
#        define random rand
#    endif
#    define ITEM_MAX_COUNT				1
#    define ITEM_GUN_MAX_FRAME			8
#    define ITEM_MINE_MAX_FRAME			1
#    define ITEM_EXPLOSION_MAX_FRAME	10
#    define ITEM_BONUS_MAX_FRAME		8
#    define ITEM_GUN_WIDTH				50
#    define ITEM_GUN_HEIGHT				20
#    define ITEM_MINE_WIDTH				15
#    define ITEM_MINE_HEIGHT			11
#    define ITEM_EXPLOSION_WIDTH		40
#    define ITEM_EXPLOSION_HEIGHT		40
#    define ITEM_BIG_EXPLOSION_WIDTH	100
#    define ITEM_BIG_EXPLOSION_HEIGHT	100
#    define ITEM_BONUS_WIDTH			20
#    define ITEM_BONUS_HEIGHT			20

#    include "main.h"
#    ifndef PUBLIC_SERVER
#        include "image.h"
#    endif
#    include "list.h"
#    include "tux.h"
#    include "myTimer.h"

typedef struct item_struct {
	int id;
	int type;					// type of thing

	int x;						// item position 
	int y;

	int w;						// item size
	int h;

	int frame;					//number of animation
	int count;

	int author_id;
#    ifndef PUBLIC_SERVER
	image_t *img;				//picture
#    endif
} item_t;

extern bool_t item_is_inicialized();
extern void item_init();
extern item_t *item_new(int x, int y, int type, int author_id);
extern void item_get_status(void *p, int *id, int *x, int *y, int *w, int *h);
extern void item_set_status(void *p, int x, int y, int w, int h);
extern void item_replace_id(item_t * item, int id);
extern void item_add_new_item(space_t * spaceItem, int author_id);
#    ifndef PUBLIC_SERVER
extern void item_draw(item_t * p);
extern void item_draw_list(list_t * listItem);
#    endif
extern void item_event(space_t * listSpace);
extern void item_event_conglicts_shot_with_item(arena_t * arena);
extern void item_event_tux_give_item(tux_t * tux, item_t * item, space_t * spaceItem);
extern void item_tux_give_list_item(tux_t * tux, space_t * spaceItem);
extern void item_destroy(item_t * p);
extern void item_quiy();
#endif
