
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

#include "screen_choiceArena.h"
#include "screen_world.h"

static void action_saveTux(space_t *space, tux_t *tux, textFile_t *textFile)
{
	char str[STR_PROTO_SIZE];

	snprintf(str, STR_PROTO_SIZE,
		"TUX %d %d %d %d %d %d %d %d %s %d %d %d %d %d %d %d %d %d %d %d",
		tux->id, tux->x, tux->y, tux->status, tux->position ,tux->control, tux->frame, tux->score, tux->name,
		tux->gun, tux->bonus, tux->shot[GUN_SIMPLE], tux->shot[GUN_DUAL_SIMPLE],
		tux->shot[GUN_SCATTER], tux->shot[GUN_TOMMY], tux->shot[GUN_LASSER],
		tux->shot[GUN_MINE], tux->shot[GUN_BOMBBALL],
		tux->bonus_time, tux->pickup_time);

	addList(textFile->text, strdup(str));
}

static void action_saveShot(space_t *space, shot_t *shot, textFile_t *textFile)
{
	char str[STR_PROTO_SIZE];

	snprintf(str, STR_PROTO_SIZE, "SHOT %d %d %d %d %d %d %d %d %d",
		shot->id, shot->x, shot->y, shot->px, shot->py,
		shot->position, shot->gun, shot->author_id, shot->isCanKillAuthor);

	addList(textFile->text, strdup(str));
}

static void action_saveItem(space_t *space, item_t *item, textFile_t *textFile)
{
	char str[STR_PROTO_SIZE];

	snprintf(str, STR_PROTO_SIZE, "ITEM %d %d %d %d %d %d %d",
		item->id, item->type, item->x, item->y,
		item->count, item->frame, item->author_id);

	addList(textFile->text, strdup(str));
}

static void saveContextArenaToTextFile(textFile_t *textFile, arena_t *arena)
{
	char str[STR_PROTO_SIZE];

	sprintf(str, "ARENA %s %d %d",
		getArenaNetName(getChoiceArena()),
		arena->countRound,
		arena->max_countRound);

	addList(textFile->text, strdup(str));

	actionSpace(arena->spaceTux, action_saveTux, textFile);
	actionSpace(arena->spaceShot, action_saveShot, textFile);
	actionSpace(arena->spaceItem, action_saveItem, textFile);
}

void saveArena(char *filename, arena_t *arena)
{
	textFile_t *textFile;
	char path[STR_PATH_SIZE];

	sprintf(path, "%s/%s.sav", getHomeDirector(), filename);
#ifdef DEBUG
	printf(_("Saving game to: \"%s\"\n"), path);
#endif
	textFile = newTextFile(path);

	if( textFile != NULL )
	{
		saveContextArenaToTextFile(textFile, arena);
		saveTextFile(textFile);
		destroyTextFile(textFile);
	}
	else
	{
		fprintf(stderr, _("I was unable to save: \"%s\"\n"), path);
	}
}

static arena_t* loadArenaFromLine(char *line)
{
	char cmd[STR_SIZE];
	char name[STR_SIZE];
	int countRound;
	int max_countRound;
	arena_t *arena;

	sscanf(line, "%s %s %d %d", cmd, name, &countRound, &max_countRound);

	setWorldArena( getArenaFileFormNetName(name) );

	arena = getCurrentArena();
	arena->max_countRound = max_countRound;
	arena->countRound = countRound;

	return arena;
}

static void loadTuxFromLine(char *line, arena_t *arena)
{
	char cmd[STR_PROTO_SIZE];
	char name[STR_NAME_SIZE];
	int id, x, y, status, position, frame, score, control;
	int myGun, myBonus;
	int  gun1, gun2, gun3, gun4, gun5, gun6, gun7;
	int time1, time2;
	tux_t *tux;

	sscanf(line, "%s %d %d %d %d %d %d %d %d %s %d %d %d %d %d %d %d %d %d %d %d",
	cmd, &id, &x, &y, &status, &position, &control, &frame, &score, name,
	&myGun, &myBonus, &gun1, &gun2, &gun3, &gun4, &gun5, &gun6, &gun7, &time1, &time2);

	tux = newTux();
	replaceTuxID(tux, id);
	addObjectToSpace(arena->spaceTux, tux);

	tux->control = control;

	if( tux->control == TUX_CONTROL_KEYBOARD_RIGHT )
	{
		setControlTux(tux, TUX_CONTROL_KEYBOARD_RIGHT);
	}

	if( tux->control == TUX_CONTROL_KEYBOARD_LEFT )
	{
		setControlTux(tux, TUX_CONTROL_KEYBOARD_LEFT);
	}

	if( tux->control == TUX_CONTROL_AI )
	{
		loadModule("libmodAI");
		setControlTux(tux, TUX_CONTROL_KEYBOARD_LEFT);
	}

	moveObjectInSpace(arena->spaceTux, tux, x, y);
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

static void loadShotFromLine(char *line, arena_t *arena)
{
	char cmd[STR_PROTO_SIZE];
	int x, y, px, py, position, gun, shot_id, author_id, isCanKillAuthor;
	shot_t *shot;

	sscanf(line, "%s %d %d %d %d %d %d %d %d %d",
		cmd, &shot_id, &x, &y, &px, &py, &position, &gun, &author_id, &isCanKillAuthor);

	shot = newShot(x, y, px, py, gun, author_id);

	replaceShotID(shot, shot_id);
	shot->isCanKillAuthor = isCanKillAuthor;
	shot->position = position;

	if( shot->gun == GUN_LASSER )
	{
		transformOnlyLasser(shot);
	}

	addObjectToSpace(arena->spaceShot, shot);
}

static void loadItemFromLine(char *line, arena_t *arena)
{
	char cmd[STR_PROTO_SIZE];
	int id, type, x, y, count, frame, author_id, check_id;
	item_t *item;

	sscanf(line, "%s %d %d %d %d %d %d %d %d",
		cmd, &id, &type, &x, &y, &count, &frame, &author_id, &check_id);

	item = newItem(x, y, type, author_id);

	replaceItemID(item, id);
	item->count = count;
	item->frame = frame;

	addObjectToSpace(arena->spaceItem, item);
}

static arena_t* loadContextArenaFromTextFile(textFile_t *textFile)
{
	arena_t *arena;
	int i;

	arena = NULL;

	for( i = 0 ; i < textFile->text->count ; i++ )
	{
		char *line;

		line = (char *)textFile->text->list[i];

		if( strncmp(line, "ARENA", 5) == 0 )
		{
			arena = loadArenaFromLine(line);
		}

		if( strncmp(line, "TUX", 3) == 0 )
		{
			loadTuxFromLine(line, arena);
		}

		if( strncmp(line, "SHOT", 4) == 0 )
		{
			loadShotFromLine(line, arena);
		}

		if( strncmp(line, "ITEM", 4) == 0 )
		{
			loadItemFromLine(line, arena);
		}
	}
	
	return arena;
}

void loadArena(char *filename)
{
	textFile_t *textFile;
	char path[STR_PATH_SIZE];

	sprintf(path, "%s/%s", getHomeDirector(), filename);
#ifdef DEBUG
	printf(_("Loadig game from file: \"%s\"\n"), path);
#endif
	textFile = loadTextFile(path);

	if( textFile != NULL )
	{
		loadContextArenaFromTextFile(textFile);
		destroyTextFile(textFile);
	}
	else
	{
		fprintf(stderr, _("Unable to load save: \"%s\"\n"), path);
	}
}
