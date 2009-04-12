
#ifndef SHOT_H
#    define SHOT_H
#    ifdef __WIN32__
#        define random rand
#    endif

#    include "main.h"
#    include "list.h"
#    include "tux.h"
#    include "arena.h"
#    ifndef PUBLIC_SERVER
#        include "image.h"
#    endif

typedef struct shot_struct {
	int id;
	int x;
	int y;
	int w;
	int h;
	int px;
	int py;
	int position;
	int gun;
	int author_id;
	bool_t isCanKillAuthor;
	bool_t del;
#    ifndef PUBLIC_SERVER
	image_t *img;
#    endif
} shot_t;

extern bool_t shot_is_inicialized();
extern void shot_init();
extern shot_t *shot_new(int x, int y, int px, int py, int gun, int author_id);
extern shot_t *shot_get_id(list_t * listShot, int id);
extern void shot_replace_id(shot_t * shot, int id);
extern void shot_get_status(void *p, int *id, int *x, int *y, int *w, int *h);
extern void shot_set_status(void *p, int x, int y, int w, int h);
#    ifndef PUBLIC_SERVER
extern void shot_draw(shot_t * p);
extern void shot_draw_list(list_t * listShot);
#    endif
extern int shot_is_conflict_with_list_shot(list_t * listShot, int x, int y, int w,
								  int h);
extern void shot_event_move_list(arena_t * arena);
extern void shot_check_is_tux_screen(arena_t * arena);
extern void shot_bound_bombBall(shot_t * shot);
extern void shot_transform_lasser(shot_t * shot);
extern void shot_destroy(shot_t * p);
extern void shot_quit();
#endif
