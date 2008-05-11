
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "main.h"
#include "list.h"
#include "textFile.h"
#include "configFile.h"
#include "arenaFile.h"
#include "arena.h"
#include "tux.h"
#include "shot.h"
#include "wall.h"
#include "pipe.h"
#include "teleport.h"
#include "item.h"
#include "director.h"
#include "path.h"
#include "string_length.h"

#ifndef PUBLIC_SERVER
#include "image.h"
#include "music.h"
#include "fake_audio.h"
#endif

static list_t *listArenaFile;

static bool_t isArenaFileInit = FALSE;

bool_t isAreaFileInicialized()
{
	return isArenaFileInit;
}

#ifndef PUBLIC_SERVER

static void cmd_loadImage(char *line)
{
	char str_file[STR_SIZE];
	char str_name[STR_SIZE];
	char str_alpha[STR_SIZE];

	if( getValue(line, "file", str_file, STR_SIZE) != 0 )return;
	if( getValue(line, "name", str_name, STR_SIZE) != 0 )return;
	if( getValue(line, "alpha", str_alpha, STR_SIZE) != 0 )return;

	addImageData(str_file, isYesOrNO(str_alpha), str_name, IMAGE_GROUP_USER);
}

static void cmd_loadMusic(char *line)
{
	char str_file[STR_SIZE];
	char str_name[STR_SIZE];

	if( getValue(line, "file", str_file, STR_SIZE) != 0 )return;
	if( getValue(line, "name", str_name, STR_SIZE) != 0 )return;

	addMusic(str_file, str_name, MUSIC_GROUP_USER);
}

static void cmd_background(arena_t *arena, char *line)
{
	char str_image[STR_SIZE];

	if( getValue(line, "image", str_image, STR_SIZE) != 0 )return;

	arena->background = getImage(IMAGE_GROUP_USER, str_image);
}

static void cmd_playMusic(arena_t *arena, char *line)
{
	char str_music[STR_SIZE];

	if( getValue(line, "music", str_music, STR_SIZE) != 0 )return;

	strcpy(arena->music, str_music);
}

#endif

static void cmd_wall(arena_t *arena, char *line)
{
	char str_x[STR_NUM_SIZE];
	char str_y[STR_NUM_SIZE];
	char str_img_x[STR_NUM_SIZE];
	char str_img_y[STR_NUM_SIZE];
	char str_w[STR_NUM_SIZE];
	char str_h[STR_NUM_SIZE];
	char str_layer[STR_NUM_SIZE];
	char str_image[STR_SIZE];
	wall_t *new;

	if( getValue(line, "x", str_x, STR_NUM_SIZE) != 0 )return;
	if( getValue(line, "y", str_y, STR_NUM_SIZE) != 0 )return;
	if( getValue(line, "w", str_w, STR_NUM_SIZE) != 0 )return;
	if( getValue(line, "h", str_h, STR_NUM_SIZE) != 0 )return;
	if( getValue(line, "img_x", str_img_x, STR_NUM_SIZE) != 0 )return;
	if( getValue(line, "img_y", str_img_y, STR_NUM_SIZE) != 0 )return;
	if( getValue(line, "layer", str_layer, STR_NUM_SIZE) != 0 )return;
	if( getValue(line, "image", str_image, STR_SIZE) != 0 )return;

#ifndef PUBLIC_SERVER
	new = newWall(atoi(str_x), atoi(str_y),
			atoi(str_w), atoi(str_h),
			atoi(str_img_x), atoi(str_img_y),
			atoi(str_layer), getImage(IMAGE_GROUP_USER, str_image) );
#endif

#ifdef PUBLIC_SERVER
	new = newWall(atoi(str_x), atoi(str_y),
			atoi(str_w), atoi(str_h),
			atoi(str_img_x), atoi(str_img_y),
			atoi(str_layer) );
#endif

	addList(arena->listWall, new);
}

static void cmd_teleport(arena_t *arena, char *line)
{
	char str_x[STR_NUM_SIZE];
	char str_y[STR_NUM_SIZE];
	char str_w[STR_NUM_SIZE];
	char str_h[STR_NUM_SIZE];
	char str_layer[STR_NUM_SIZE];
	char str_image[STR_SIZE];
	teleport_t *new;

	if( getValue(line, "x", str_x, STR_NUM_SIZE) != 0 )return;
	if( getValue(line, "y", str_y, STR_NUM_SIZE) != 0 )return;
	if( getValue(line, "w", str_w, STR_NUM_SIZE) != 0 )return;
	if( getValue(line, "h", str_h, STR_NUM_SIZE) != 0 )return;
	if( getValue(line, "layer", str_layer, STR_NUM_SIZE) != 0 )return;
	if( getValue(line, "image", str_image, STR_SIZE) != 0 )return;

#ifndef PUBLIC_SERVER
	new = newTeleport(atoi(str_x), atoi(str_y),
			atoi(str_w), atoi(str_h),
			atoi(str_layer), getImage(IMAGE_GROUP_USER, str_image) );
#endif

#ifdef PUBLIC_SERVER
	new = newTeleport(atoi(str_x), atoi(str_y),
			atoi(str_w), atoi(str_h),
			atoi(str_layer) );
#endif

	addList(arena->listTeleport, new);
}

static void cmd_pipe(arena_t *arena, char *line)
{
	char str_x[STR_NUM_SIZE];
	char str_y[STR_NUM_SIZE];
	char str_w[STR_NUM_SIZE];
	char str_h[STR_NUM_SIZE];
	char str_id[STR_NUM_SIZE];
	char str_id_out[STR_NUM_SIZE];
	char str_position[STR_NUM_SIZE];
	char str_layer[STR_NUM_SIZE];
	char str_image[STR_SIZE];
	pipe_t *new;

	if( getValue(line, "x", str_x, STR_NUM_SIZE) != 0 )return;
	if( getValue(line, "y", str_y, STR_NUM_SIZE) != 0 )return;
	if( getValue(line, "w", str_w, STR_NUM_SIZE) != 0 )return;
	if( getValue(line, "h", str_h, STR_NUM_SIZE) != 0 )return;
	if( getValue(line, "id", str_id, STR_NUM_SIZE) != 0 )return;
	if( getValue(line, "id_out", str_id_out, STR_NUM_SIZE) != 0 )return;
	if( getValue(line, "position", str_position, STR_NUM_SIZE) != 0 )return;
	if( getValue(line, "layer", str_layer, STR_NUM_SIZE) != 0 )return;
	if( getValue(line, "image", str_image, STR_SIZE) != 0 )return;

#ifndef PUBLIC_SERVER
	new = newPipe(atoi(str_x), atoi(str_y),
			atoi(str_w), atoi(str_h),
			atoi(str_layer), atoi(str_id), atoi(str_id_out), atoi(str_position),
			getImage(IMAGE_GROUP_USER, str_image) );
#endif

#ifdef PUBLIC_SERVER
	new = newPipe(atoi(str_x), atoi(str_y),
			atoi(str_w), atoi(str_h),
			atoi(str_layer), atoi(str_id), atoi(str_id_out), atoi(str_position) );
#endif

	addList(arena->listPipe, new);
}

int getArenaCount()
{
	return listArenaFile->count;
}

static char *getArenaStatus(int id, char *s)
{
	textFile_t *ts;
	int len;
	int i;

	ts = (textFile_t *) listArenaFile->list[id];
	len = strlen(s);

	for( i = 0 ; i < ts->text->count ; i++ )
	{
		char *line;
		line = (char *) (ts->text->list[i]);

		if( strncmp(line, s, len) == 0 )
		{
			return line+len+1;
		}
	}

	return NULL;
}

char *getArenaName(int id)
{
	char *ret;

	ret = getArenaStatus(id, "name");

	return ( ret != NULL ? ret : "arena_no_name" );
}

char *getArenaNetName(int id)
{
	char *ret;

	ret = getArenaStatus(id, "netName");

	return ( ret != NULL ? ret : "arena_no_net_name" );
}

int getArenaIdFormNetName(char *s)
{
	int i;

	for( i = 0 ; i < listArenaFile->count ; i++ )
	{
		if( strcmp(getArenaNetName(i), s) == 0 )
		{
			return i;
		}
	}

	return -1;
}

char *getArenaImage(int id)
{
	return getArenaStatus(id, "screen");
}

arena_t* getArena(int id)
{
	textFile_t *ts;
	arena_t *arena;
	int i;

	ts = (textFile_t *) listArenaFile->list[id];
	arena = newArena();

	for( i = 0 ; i < ts->text->count ; i++ )
	{
		char *line;
		line = (char *) (ts->text->list[i]);

#ifndef PUBLIC_SERVER
		if( strncmp(line, "loadImage", 9) == 0 )cmd_loadImage(line);
		if( strncmp(line, "loadMusic", 9) == 0 )cmd_loadMusic(line);
		if( strncmp(line, "background", 10) == 0 )cmd_background(arena, line);
		if( strncmp(line, "playMusic", 9) == 0 )cmd_playMusic(arena, line);
#endif

		if( strncmp(line, "wall", 4) == 0 )cmd_wall(arena, line);
		if( strncmp(line, "teleport", 8) == 0 )cmd_teleport(arena, line);
		if( strncmp(line, "pipe", 4) == 0 )cmd_pipe(arena, line);
	}

	return arena;
}

void initArenaFile()
{
	director_t *p;
	int i;

#ifndef PUBLIC_SERVER
	assert( isImageInicialized() == TRUE );
#endif

	isArenaFileInit = TRUE;
	listArenaFile  = newList();

	p = loadDirector(PATH_ARENA);

	for( i = 0 ; i < p->list->count ; i++ )
	{
		char *line;

		line = (char *)( p->list->list[i] );

		if( strstr(line, ".map") != NULL && strstr(line, "~") == NULL )
		{
			char path[STR_PATH_SIZE];

			sprintf(path, PATH_ARENA "%s", line);

			printf("load map %s\n", line);
		
			accessExistFile(path);
			addList(listArenaFile, loadTextFile(path));
		}
	}

	destroyDirector(p);
}

void quitArenaFile()
{
	isArenaFileInit = FALSE;
	destroyListItem(listArenaFile, destroyTextFile);
}

