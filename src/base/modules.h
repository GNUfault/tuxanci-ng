#ifndef MODULE_H
#define MODULE_H

#include "tux.h"
#include "shot.h"
#include "arena.h"
#include "proto.h"
#include "myTimer.h"

#ifndef PUBLIC_SERVER
#include "image.h"
#endif /* PUBLIC_SERVER */

#ifndef __WIN32__
	#ifndef APPLE
		#define MODULE_TYPE	".so"
	#else
		#define MODULE_TYPE	".dylib"
	#endif
#else
	#define MODULE_TYPE	".dll"
#endif

typedef struct export_fce_s {
	int (*fce_getValue) (char *line, char *env, char *val, int len);

#ifndef PUBLIC_SERVER
	image_t *(*fce_image_get) (char *group, char *name);

	void (*fce_addLayer) (image_t *img, int x, int y, int px, int py,
					    int w, int h, int player);
#endif

	int (*fce_net_multiplayer_get_game_type) ();
	int (*fce_module_load_dep) (char *name);
	void (*fce_share_function_add) (char *name, void *function);
	void *(*fce_share_function_get) (char *name);

	void (*fce_tux_get_proportion) (tux_t *tux, int *x, int *y, int *w, int *h);
	void (*fce_tux_set_proportion) (tux_t *tux, int x, int y);
	tux_t *(*fce_getTuxID) (list_t *listTux, int id);
	void (*fce_tux_action) (tux_t *tux, int action);

	arena_t *(*fce_arena_get_current) ();
	int (*fce_arena_conflict_space) (int x1, int y1, int w1, int h1,
					 int x2, int y2, int w2, int h2);
	int (*fce_arena_is_free_space) (arena_t *arena, int x, int y, int w, int h);
	void (*fce_arena_find_free_space) (arena_t *arena, int *x, int *y, int w, int h);

	void (*fce_proto_send_del_server)(int type, client_t *client, int id);
	void (*fce_proto_send_module_server) (int type, client_t *client, char *msg);
	void (*fce_proto_send_module_client) (char *msg);

	 my_time_t(*fce_timer_get_current_time) ();

	void (*fce_shot_destroy) (shot_t *p);
	void (*fce_shot_bound_bombBall) (shot_t *shot);
	void (*fce_shot_transform_lasser) (shot_t *shot);
} export_fce_t;

typedef struct module_s {
	char *name;
#ifndef __WIN32__
	void *image;
#else /* __WIN32__ */
	HINSTANCE *image;
#endif /* __WIN32__ */

	int (*fce_init) (export_fce_t *p);
#ifndef PUBLIC_SERVER
	int (*fce_draw) (int x, int y, int w, int h);
#endif /* PUBLIC_SERVER */
	int (*fce_event) ();
	int (*fce_isConflict) (int x, int y, int w, int h);
	void (*fce_cmd) (char *line);
	void (*fce_recvMsg) (char *msg);
	int (*fce_destroy) ();

} module_t;

extern void module_init();
extern int module_load(char *name);
extern int module_load_dep(char *name);
#ifndef PUBLIC_SERVER
extern void module_draw(int x, int y, int w, int h);
#endif /* PUBLIC_SERVER */
extern void module_event();
extern void module_cmd(char *s);
extern int module_is_conflict(int x, int y, int w, int h);
extern int module_recv_msg(char *msg);
extern void module_quit();

#endif /* MODULES_H */
