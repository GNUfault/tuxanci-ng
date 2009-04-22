
#include <stdlib.h>
#include <assert.h>

#include "main.h"
#include "list.h"
#include "myTimer.h"
#include "tux.h"
#include "space.h"
#include "arena.h"

#include "interface.h"
#include "term.h"
#include "font.h"

static list_t *listText;
static bool_t activeTerm;
static my_time_t lastActive;
static my_time_t lastRefresh;

void term_init()
{
	listText = list_new();

	activeTerm = FALSE;
	lastActive = timer_get_current_time();
	lastRefresh = lastActive;
}

static char *getStrGun(int gun)
{
	switch (gun) {
		case GUN_SIMPLE:
			return "revolver";
		case GUN_DUAL_SIMPLE:
			return "dual revolver";
		case GUN_SCATTER:
			return "scatter";
		case GUN_TOMMY:
			return "tommy";
		case GUN_LASSER:
			return "lasser";
		case GUN_MINE:
			return "mine";
		case GUN_BOMBBALL:
			return "bombball";
		default:
			return "none";
	}

	return "gun_unknow";
}

static char *getStrBonus(int bonus)
{
	switch (bonus) {
		case BONUS_SPEED:
			return "speed";
		case BONUS_SHOT:
			return "shot";
		case BONUS_TELEPORT:
			return "teleport";
		case BONUS_GHOST:
			return "ghost";
		case BONUS_4X:
			return "4X";
		case BONUS_HIDDEN:
			return "hidden";
		default:
			return "none";
	}

	return "bonus_unknow";
}

static void action_refreshTerm(space_t * space, tux_t * tux, void *p)
{
	char str[STR_SIZE];

	UNUSET(space);
	UNUSET(p);

	sprintf(str,
			"name: %s "
			"score: %d "
			"gun: %s "
			"shot: %d "
			"bonus: %s",
			tux->name, tux->score,
			getStrGun(tux->gun), tux->shot[tux->gun], getStrBonus(tux->bonus)
		);

	list_add(listText, strdup(str));
}

static void refreshTerm()
{
	arena_t *arena;

	arena = arena_get_current();

	list_destroy_item(listText, free);
	listText = list_new();

	space_action(arena->spaceTux, action_refreshTerm, NULL);

	//printf("refresh term..\n");
}

void term_draw()
{
	int i;

	if (activeTerm == FALSE) {
		return;
	}

	for (i = 0; i < listText->count; i++) {
		char *line;

		line = (char *) listText->list[i];
		font_draw(line, 10, 10 + i * 20, COLOR_WHITE);
	}
}

static void switchTerm()
{
	if (activeTerm == TRUE) {
		activeTerm = FALSE;
	} else {
		activeTerm = TRUE;
	}
}

void term_event()
{
	my_time_t currentTime;
	Uint8 *mapa;

	mapa = SDL_GetKeyState(NULL);

	currentTime = timer_get_current_time();

	if (currentTime - lastRefresh > TERM_REFRESH_TIME_INTERVAL) {
		lastRefresh = currentTime;
		refreshTerm();
	}

	if (mapa[SDLK_TAB] == SDL_PRESSED) {
		if (currentTime - lastActive > TERM_ACTIVE_TIME_INTERVAL) {
			lastActive = currentTime;
			switchTerm();
		}
	}
}

void term_quit()
{
	assert(listText != NULL);
	list_destroy_item(listText, free);
}
