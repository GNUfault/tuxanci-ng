
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "main.h"
#include "arena.h"
#include "list.h"
#include "tux.h"
#include "shot.h"
#include "item.h"
#include "myTimer.h"
#include "modules.h"

#ifndef PUBLIC_SERVER
#    include "hotKey.h"
#    include "world.h"
#    include "setting.h"
#    include "layer.h"
#endif

static arena_t *currentArena;
static int splitType;

#ifndef PUBLIC_SERVER
static int isBigArena(arena_t * arena)
{
	if (arena->w > WINDOW_SIZE_X || arena->h > WINDOW_SIZE_Y) {
		return 1;
	} else {
		return 0;
	}
}

static int isTuxNear(tux_t * tux1, tux_t * tux2)
{
	if (abs(tux1->x - tux2->x) < WINDOW_SIZE_X &&
	    abs(tux1->y - tux2->y) < WINDOW_SIZE_Y) {
		return 1;
	}

	return 0;
}
#endif

void arena_set_current(arena_t * p)
{
	currentArena = p;
}

arena_t *arena_get_current()
{
	return currentArena;
}

void hotkey_splitArena()
{
	if (splitType == SCREEN_SPLIT_VERTICAL) {
		splitType = SCREEN_SPLIT_HORIZONTAL;
	} else {
		splitType = SCREEN_SPLIT_VERTICAL;
	}
}

#ifndef PUBLIC_SERVER
void arena_init()
{
	splitType = SCREEN_SPLIT_VERTICAL;

	if (getParam("--split-vertical")) {
		splitType = SCREEN_SPLIT_VERTICAL;
		printf("SCREEN_SPLIT_VERTICAL\n");
	}

	if (getParam("--split-horizontal")) {
		splitType = SCREEN_SPLIT_HORIZONTAL;
		printf("SCREEN_SPLIT_HORIZONTAL\n");
	}

	hotKey_register(SDLK_F3, hotkey_splitArena);
}

void arena_quit()
{
	unhotKey_register(SDLK_F3);
}
#endif

arena_t *arena_new(int w, int h)
{
	arena_t *new;
	int zone_w, zone_h;

	new = malloc(sizeof(arena_t));

#ifndef PUBLIC_SERVER
	new->background = NULL;
	strcpy(new->music, "");
#endif

	new->w = w;
	new->h = h;

	new->listTimer = list_new();

	if (w > 800 || h > 600) {
		zone_w = 320;
		zone_h = 240;
	} else {
		zone_w = 80;
		zone_h = 60;
	}

	new->spaceTux = space_new(w, h, zone_w, zone_h, tux_get_status, tux_set_status);
	new->spaceItem = space_new(w, h, zone_w, zone_h, item_get_status, item_set_status);
	new->spaceShot = space_new(w, h, zone_w, zone_h, shot_get_status, shot_set_status);

	new->countRound = 0;
	new->max_countRound = 0;

	return new;
}

int
arena_conflict_space(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2)
{
	return (x1 < x2 + w2 && x2 < x1 + w1 && y1 < y2 + h2 && y2 < y1 + h1);
}

int arena__is_free_space(arena_t * arena, int x, int y, int w, int h)
{
	if (x < 0 || y < 0 || x + w > arena->w || y + h > arena->h) {
		return 0;
	}

	if (space_is_conflict_with_object(arena->spaceTux, x, y, w, h))
		return 0;

	if (space_is_conflict_with_object(arena->spaceShot, x, y, w, h))
		return 0;

	if (space_is_conflict_with_object(arena->spaceItem, x, y, w, h))
		return 0;

	if (space_is_conflict_with_object(arena->spaceShot, x, y, w, h))
		return 0;

	if (module_is_conflict(x, y, w, h))
		return 0;

	return 1;
}

void arena_find_free_space(arena_t * arena, int *x, int *y, int w, int h)
{
	int z_x;
	int z_y;

	assert(x != NULL);
	assert(y != NULL);

	do {
		z_x = random() % (arena->w - w);
		z_y = random() % (arena->h - h);
	} while (arena__is_free_space(arena, z_x, z_y, w, h) == 0);

	*x = z_x;
	*y = z_y;
}

void arena_get_center_screen(int *screen_x, int *screen_y, int x, int y, int screen_size_x, int screen_size_y)
{
	arena_t *arena;

	arena = arena_get_current();

	*screen_x = x - screen_size_x / 2;
	*screen_y = y - screen_size_y / 2;

	if (*screen_x < 0) {
		*screen_x = 0;
	}

	if (*screen_y < 0) {
		*screen_y = 0;
	}

	if (*screen_x + screen_size_x >= arena->w) {
		*screen_x = arena->w - screen_size_x;
	}

	if (*screen_y + screen_size_y >= arena->h) {
		*screen_y = arena->h - screen_size_y;
	}
}

#ifndef PUBLIC_SERVER

static void drawBackground(arena_t * arena, int screen_x, int screen_y)
{

	if (isBigArena(arena)) {
		int i, j;

		for (i = screen_y / arena->background->h;
		     i <= screen_y / arena->background->h + WINDOW_SIZE_Y / arena->background->h + 1; i++) {

			for (j = screen_x / arena->background->w;
			     j <= screen_x / arena->background->w + WINDOW_SIZE_X / arena->background->w + 1; j++) {

				addLayer(arena->background, j * arena->background->w,
					 i * arena->background->h, 0, 0, arena->background->w,
					 arena->background->h, -100);

				//count++;
			}
		}
	} else {
		addLayer(arena->background, 0, 0, 0, 0, arena->background->w, arena->background->h, -100);
	}
}

static void action_tux_draw(space_t * space, tux_t * tux, void *p)
{
	tux_draw(tux);
}

static void action_item_draw(space_t * space, item_t * item, void *p)
{
	item_draw(item);
}

static void action_shot_draw(space_t * space, shot_t * shot, void *p)
{
	shot_draw(shot);
}

static void drawObjects(arena_t * arena, int screen_x, int screen_y)
{
	space_action_from_location(arena->spaceTux, action_tux_draw, NULL, screen_x, screen_y, WINDOW_SIZE_X, WINDOW_SIZE_Y);
	space_action_from_location(arena->spaceItem, action_item_draw, NULL, screen_x, screen_y, WINDOW_SIZE_X, WINDOW_SIZE_Y);
	space_action_from_location(arena->spaceShot, action_shot_draw, NULL, screen_x, screen_y, WINDOW_SIZE_X, WINDOW_SIZE_Y);

	module_draw(screen_x, screen_y, WINDOW_SIZE_X, WINDOW_SIZE_Y);
}

static void drawSplitArenaForTux(arena_t * arena, tux_t * tux, int location_x, int location_y)
{
	int screen_x, screen_y;

	switch (splitType) {
		case SCREEN_SPLIT_HORIZONTAL:
			arena_get_center_screen(&screen_x, &screen_y, tux->x, tux->y, WINDOW_SIZE_X, WINDOW_SIZE_Y / 2);
			break;
		case SCREEN_SPLIT_VERTICAL:
			arena_get_center_screen(&screen_x, &screen_y, tux->x, tux->y, WINDOW_SIZE_X / 2, WINDOW_SIZE_Y);
			break;
		default:
			screen_x = -1;
			screen_y = -1;
			assert(!"stupd error ");
			break;
	}

	drawBackground(arena, screen_x, screen_y);
	drawObjects(arena, screen_x, screen_y);

	switch (splitType) {
		case SCREEN_SPLIT_HORIZONTAL:
			layer_draw_slpit(location_x, location_y, screen_x, screen_y, WINDOW_SIZE_X, WINDOW_SIZE_Y / 2);
			break;
		case SCREEN_SPLIT_VERTICAL:
			layer_draw_slpit(location_x, location_y, screen_x, screen_y, WINDOW_SIZE_X / 2, WINDOW_SIZE_Y);
			break;
		default:
			screen_x = -1;
			screen_y = -1;
			assert(!"stupd error ");
			break;
	}
}

void drawSplitArena(arena_t * arena)
{
	tux_t *tux = NULL;

	tux = word_get_control_tux(TUX_CONTROL_KEYBOARD_RIGHT);

	if (tux != NULL) {
		switch (splitType) {
			case SCREEN_SPLIT_HORIZONTAL:
				drawSplitArenaForTux(arena, tux, 0, WINDOW_SIZE_Y / 2);
				break;
			case SCREEN_SPLIT_VERTICAL:
				drawSplitArenaForTux(arena, tux, WINDOW_SIZE_X / 2, 0);
				break;
		}
	}

	tux = word_get_control_tux(TUX_CONTROL_KEYBOARD_LEFT);

	if (tux != NULL) {
		drawSplitArenaForTux(arena, tux, 0, 0);
	}
}

void drawCenterArena(arena_t * arena, int x, int y)
{
	int screen_x, screen_y;

	arena_get_center_screen(&screen_x, &screen_y, x, y, WINDOW_SIZE_X, WINDOW_SIZE_Y);
	drawBackground(arena, screen_x, screen_y);
	drawObjects(arena, screen_x, screen_y);
	layer_draw_center(x, y);
}

void drawSimpleArena(arena_t * arena)
{
	int screen_x, screen_y;
	tux_t *tux = NULL;

	tux = word_get_control_tux(TUX_CONTROL_KEYBOARD_RIGHT);

	if (tux == NULL) {
		return;
	}

	screen_x = 0;
	screen_y = 0;

	drawBackground(arena, screen_x, screen_y);
	drawObjects(arena, screen_x, screen_y);
	layer_draw_all(tux->x, tux->y);
}

void arena_draw(arena_t * arena)
{
	if (isBigArena(arena) &&
	    netMultiplayer_get_game_type() == NET_GAME_TYPE_NONE && !setting_is_ai()) {
		tux_t *tux1;
		tux_t *tux2;

		tux1 = word_get_control_tux(TUX_CONTROL_KEYBOARD_RIGHT);
		tux2 = word_get_control_tux(TUX_CONTROL_KEYBOARD_LEFT);

		if (isTuxNear(tux1, tux2)) {
			drawCenterArena(arena, (tux1->x + tux2->x) / 2, (tux1->y + tux2->y) / 2);
		} else {
			drawSplitArena(arena);
		}

		return;
	}

	if (isBigArena(arena)) {
		tux_t *tux = NULL;

		tux = word_get_control_tux(TUX_CONTROL_KEYBOARD_RIGHT);

		if (tux == NULL) {
			return;
		}

		drawCenterArena(arena, tux->x, tux->y);
		return;
	}

	drawSimpleArena(arena);
}

#endif

static void action_tux(space_t * space, tux_t * tux, void *p)
{
	tux_event(tux);
}

void arena_event(arena_t * arena)
{
	int i;

	for (i = 0; i < 8; i++) {
		shot_check_is_tux_screen(arena);

		item_event_conglicts_shot_with_item(arena);
		tux_conflict_woth_shot(arena);
		module_event();

		shot_event_move_list(arena);
	}

	item_event(arena->spaceItem);

	space_action(arena->spaceTux, action_tux, NULL);

	timer_event(arena->listTimer);
}

void arena_destroy(arena_t * p)
{
	space_destroy_with_item(p->spaceTux, tux_destroy);
	space_destroy_with_item(p->spaceItem, item_destroy);
	space_destroy_with_item(p->spaceShot, shot_destroy);
	timer_destroy(p->listTimer);
	free(p);
}
