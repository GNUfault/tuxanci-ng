
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "textFile.h"
#include "homeDirector.h"

#include "arena.h"
#include "arenaFile.h"
#include "tux.h"
#include "shot.h"
#include "item.h"
#include "modules.h"

#include "saveLoad.h"

#include "choiceArena.h"
#include "world.h"

static void action_saveTux(space_t * space, tux_t * tux, textFile_t * textFile)
{
	char str[STR_PROTO_SIZE];

	snprintf(str, STR_PROTO_SIZE,
			 "TUX %d %d %d %d %d %d %d %d %s %d %d %d %d %d %d %d %d %d %d %d",
			 tux->id, tux->x, tux->y, tux->status, tux->position,
			 tux->control, tux->frame, tux->score, tux->name, tux->gun,
			 tux->bonus, tux->shot[GUN_SIMPLE], tux->shot[GUN_DUAL_SIMPLE],
			 tux->shot[GUN_SCATTER], tux->shot[GUN_TOMMY],
			 tux->shot[GUN_LASSER], tux->shot[GUN_MINE],
			 tux->shot[GUN_BOMBBALL], tux->bonus_time, tux->pickup_time);

	list_add(textFile->text, strdup(str));
}

static void
action_saveShot(space_t * space, shot_t * shot, textFile_t * textFile)
{
	char str[STR_PROTO_SIZE];

	snprintf(str, STR_PROTO_SIZE, "SHOT %d %d %d %d %d %d %d %d %d",
			 shot->id, shot->x, shot->y, shot->px, shot->py,
			 shot->position, shot->gun, shot->author_id,
			 shot->isCanKillAuthor);

	list_add(textFile->text, strdup(str));
}

static void
action_saveItem(space_t * space, item_t * item, textFile_t * textFile)
{
	char str[STR_PROTO_SIZE];

	snprintf(str, STR_PROTO_SIZE, "ITEM %d %d %d %d %d %d %d",
			 item->id, item->type, item->x, item->y,
			 item->count, item->frame, item->author_id);

	list_add(textFile->text, strdup(str));
}

static void saveContextArenaToTextFile(textFile_t * textFile, arena_t * arena)
{
	char str[STR_PROTO_SIZE];

	sprintf(str, "ARENA %s %d %d",
			arenaFile_get_net_name(choiceArena_get()),
			arena->countRound, arena->max_countRound);

	list_add(textFile->text, strdup(str));

	space_action(arena->spaceTux, action_saveTux, textFile);
	space_action(arena->spaceShot, action_saveShot, textFile);
	space_action(arena->spaceItem, action_saveItem, textFile);
}

void save_arena(char *filename, arena_t * arena)
{
	textFile_t *textFile;
	char path[STR_PATH_SIZE];

	sprintf(path, "%s/%s.sav", homeDirector_get(), filename);

	DEBUG_MSG(_("Saving game to: \"%s\"\n"), path);

	textFile = textFile_new(path);

	if (textFile != NULL) {
		saveContextArenaToTextFile(textFile, arena);
		textFile_save(textFile);
		textFile_destroy(textFile);
	} else {
		fprintf(stderr, _("I was unable to save: \"%s\"\n"), path);
	}
}

static arena_t *load_arenaFromLine(char *line)
{
	char cmd[STR_SIZE];
	char name[STR_SIZE];
	int countRound;
	int max_countRound;
	arena_t *arena;

	sscanf(line, "%s %s %d %d", cmd, name, &countRound, &max_countRound);

	word_set_arena(arenaFile_get_file_format_net_name(name));

	arena = arena_get_current();
	arena->max_countRound = max_countRound;
	arena->countRound = countRound;

	return arena;
}

static void loadTuxFromLine(char *line, arena_t * arena)
{
	char cmd[STR_PROTO_SIZE];
	char name[STR_NAME_SIZE];
	int id, x, y, status, position, frame, score, control;
	int myGun, myBonus;
	int gun1, gun2, gun3, gun4, gun5, gun6, gun7;
	int time1, time2;
	tux_t *tux;

	sscanf(line,
		   "%s %d %d %d %d %d %d %d %d %s %d %d %d %d %d %d %d %d %d %d %d",
		   cmd, &id, &x, &y, &status, &position, &control, &frame, &score,
		   name, &myGun, &myBonus, &gun1, &gun2, &gun3, &gun4, &gun5, &gun6,
		   &gun7, &time1, &time2);

	tux = tux_new();
	tux_replace_id(tux, id);
	space_add(arena->spaceTux, tux);

	tux->control = control;

	if (tux->control == TUX_CONTROL_KEYBOARD_RIGHT) {
		word_set_control_tux(tux, TUX_CONTROL_KEYBOARD_RIGHT);
	}

	if (tux->control == TUX_CONTROL_KEYBOARD_LEFT) {
		word_set_control_tux(tux, TUX_CONTROL_KEYBOARD_LEFT);
	}

	if (tux->control == TUX_CONTROL_AI) {
		module_load("libmodAI");
		word_set_control_tux(tux, TUX_CONTROL_KEYBOARD_LEFT);
	}

	space_move_object(arena->spaceTux, tux, x, y);
	tux->status = status;
	tux->position = position;
	tux->frame = frame;
	tux->score = score;
	strcpy(tux->name, name);
	tux->gun = myGun;
	tux->bonus = myBonus;
	tux->shot[GUN_SIMPLE] = gun1;
	tux->shot[GUN_DUAL_SIMPLE] = gun2;
	tux->shot[GUN_SCATTER] = gun3;
	tux->shot[GUN_TOMMY] = gun4;
	tux->shot[GUN_LASSER] = gun5;
	tux->shot[GUN_MINE] = gun6;
	tux->shot[GUN_BOMBBALL] = gun7;
	tux->bonus_time = time1;
	tux->pickup_time = time2;
}

static void loadShotFromLine(char *line, arena_t * arena)
{
	char cmd[STR_PROTO_SIZE];
	int x, y, px, py, position, gun, shot_id, author_id, isCanKillAuthor;
	shot_t *shot;

	sscanf(line, "%s %d %d %d %d %d %d %d %d %d",
		   cmd, &shot_id, &x, &y, &px, &py, &position, &gun, &author_id,
		   &isCanKillAuthor);

	shot = shot_new(x, y, px, py, gun, author_id);

	shot_replace_id(shot, shot_id);
	shot->isCanKillAuthor = isCanKillAuthor;
	shot->position = position;

	if (shot->gun == GUN_LASSER) {
		shot_transform_lasser(shot);
	}

	space_add(arena->spaceShot, shot);
}

static void loadItemFromLine(char *line, arena_t * arena)
{
	char cmd[STR_PROTO_SIZE];
	int id, type, x, y, count, frame, author_id, check_id;
	item_t *item;

	sscanf(line, "%s %d %d %d %d %d %d %d %d",
		   cmd, &id, &type, &x, &y, &count, &frame, &author_id, &check_id);

	item = item_new(x, y, type, author_id);

	item_replace_id(item, id);
	item->count = count;
	item->frame = frame;

	space_add(arena->spaceItem, item);
}

static arena_t *loadContextArenaFromTextFile(textFile_t * textFile)
{
	arena_t *arena;
	int i;

	arena = NULL;

	for (i = 0; i < textFile->text->count; i++) {
		char *line;

		line = (char *) textFile->text->list[i];

		if (strncmp(line, "ARENA", 5) == 0) {
			arena = load_arenaFromLine(line);
		}

		if (strncmp(line, "TUX", 3) == 0) {
			loadTuxFromLine(line, arena);
		}

		if (strncmp(line, "SHOT", 4) == 0) {
			loadShotFromLine(line, arena);
		}

		if (strncmp(line, "ITEM", 4) == 0) {
			loadItemFromLine(line, arena);
		}
	}

	return arena;
}

void load_arena(char *filename)
{
	textFile_t *textFile;
	char path[STR_PATH_SIZE];

	sprintf(path, "%s/%s", homeDirector_get(), filename);

	DEBUG_MSG(_("Loadig game from file: \"%s\"\n"), path);

	textFile = textFile_load(path);

	if (textFile != NULL) {
		loadContextArenaFromTextFile(textFile);
		textFile_destroy(textFile);
	} else {
		fprintf(stderr, _("Unable to load save: \"%s\"\n"), path);
	}
}
