
#ifndef TUX_H

#    define TUX_H

#    include "main.h"
#    include "list.h"
#    include "arena.h"

#    define TUX_STEP 7
#    define TUX_LAYER 0

#    define TUX_MAX_ANIMATION_FRAME 10
#    define TUX_KEY 1

#    define TUX_LOCATE_UNKNOWN -100

#    define TUX_MAX_PICKUP	50
#    define TUX_MAX_BONUS	500

#    define TUX_TIME_SPAWN		2000
#    define TUX_TIME_CAN_SHOT	500
#    define TUX_TIME_CAN_SWITCH_GUN	100

#    define TUX_STATUS_ALIVE	0
#    define TUX_STATUS_DEAD		1
#    define TUX_STATUS_PAUZED	2

#    define TUX_CONTROL_NONE		0
#    define TUX_CONTROL_KEYBOARD_LEFT	1
#    define TUX_CONTROL_KEYBOARD_RIGHT	2
#    define TUX_CONTROL_NET			3
#    define TUX_CONTROL_AI			4

#    define TUX_WIDTH	30
#    define TUX_HEIGHT	30

#    define TUX_IMG_WIDTH	40
#    define TUX_IMG_HEIGHT	54

#    define TUX_UP			8
#    define TUX_LEFT		4
#    define TUX_RIGHT		6
#    define TUX_DOWN		2
#    define POSITION_UNKNOWN	-1

#    define TUX_SHOT	5
#    define TUX_SWITCH_GUN	0


#    define GUN_NONE	-1

#    define GUN_COUNT	7
#    define GUN_MAX_SHOT	5

#    define BONUS_COUNT	6
#    define ITEM_COUNT	16


#    define SHOT_CHECK_AREA	25

#    define GUN_SIMPLE		0
#    define GUN_DUAL_SIMPLE		1
#    define GUN_SCATTER		2
#    define GUN_TOMMY		3
#    define GUN_LASSER		4
#    define GUN_MINE		5
#    define GUN_BOMBBALL		6

#    define ITEM_MINE		7
#    define ITEM_EXPLOSION		8
#    define ITEM_BIG_EXPLOSION	9

#    define BONUS_NONE	-1
#    define BONUS_SPEED	10
#    define BONUS_SHOT	11
#    define BONUS_TELEPORT	12
#    define BONUS_GHOST	13
#    define BONUS_4X	14
#    define BONUS_HIDDEN	15

typedef struct tux_struct {
	int id;

	int x;
	int y;

	char name[STR_NAME_SIZE];

	int status;
	int control;
	int position;
	int score;

	int gun;
	int shot[GUN_COUNT];
	int round;
	int bonus;

	int bonus_time;
	int pickup_time;

	bool_t isCanShot;
	bool_t isCanSwitchGun;

	int frame;

	void *client;
} tux_t;

extern bool_t tux_is_inicialized();
extern void tux_init();
extern tux_t *tux_new();
extern void tux_set_name(tux_t * tux, char *name);
extern bool_t tux_is_any_gun(tux_t * tux);
extern void tux_get_course(int n, int *x, int *y);
extern void tux_draw(tux_t * tux);
//extern void drawListTux(list_t *listTux);
extern void tux_event_tux_is_dead(tux_t * tux);
//extern tux_t* isConflictWithListTux(list_t *listTux, int x, int y, int w, int h);
//extern int isConflictTuxWithListTux(tux_t *tux, list_t *listTux);
extern void tux_conflict_woth_shot(arena_t * arena);
//extern void eventConflictTuxWithTeleport(list_t *listTux, list_t *listTeleport);
extern void tux_teleport(tux_t * tux);
extern void tux_action(tux_t * tux, int action);
extern void tux_event(tux_t * tux);
//extern void eventListTux(list_t *listTux);
//extern tux_t* getTuxID(list_t *listTux, int id);
extern void tux_get_proportion(tux_t * tux, int *x, int *y, int *w, int *h);
extern void tux_replace_id(tux_t * tux, int id);
extern void tux_set_proportion(tux_t * tux, int x, int y);
extern void tux_get_status(void *p, int *id, int *x, int *y, int *w, int *h);
extern void tux_set_status(void *p, int x, int y, int w, int h);
extern void tux_destroy(tux_t * tux);
extern void tux_quit();

#endif
