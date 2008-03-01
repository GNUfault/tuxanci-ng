
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "main.h"
#include "list.h"
#include "textFile.h"
#include "arenaFile.h"
#include "arena.h"
#include "tux.h"
#include "shot.h"
#include "wall.h"
#include "item.h"
#include "director.h"
#include "path.h"
#include "string_length.h"
#include "image.h"
#include "music.h"

static list_t *listArenaFile;

static bool_t isArenaFileInit = FALSE;

bool_t isAreaFileInicialized()
{
	return isArenaFileInit;
}

static int setValue(char *line, char *env, char *val, int len)
{
	char *offset_env;
	char *offset_val_begin;
	char *offset_val_end;
	char clone_env[STR_SIZE];
	int val_len;

	strcpy(clone_env, " ");
	strcat(clone_env, env);

	//printf("%s %s\n", line, env);

	offset_env = strstr(line, clone_env);
	//printf("offset_env = %s\n", offset_env);
	if( offset_env == NULL )return -1;

	offset_val_begin = strchr(offset_env, '"');
	//printf("offset_val_begin = %s\n", offset_val_begin);
	if( offset_val_begin == NULL )return -1;

	offset_val_end = strchr(offset_val_begin+1, '"');
	//printf("offset_val_end = %s\n", offset_val_end);
	if( offset_val_end == NULL )return -1;

	val_len = (int)(offset_val_end - ( offset_val_begin + 1) );
	if( val_len > len - 1 ) val_len = len - 1;
	//printf("val_len = %d\n", val_len);

	memset(val, 0, len);
	memcpy(val, offset_val_begin+1, val_len);
	//printf("val = %s\n", val);
	return 0;
}

static int isYesOrNO(char *s)
{
	if( s[0] == 'Y' || s[0] == 'y' )return 1;
	return 0;
}

static void cmd_loadImage(char *line)
{
	char str_file[STR_SIZE];
	char str_name[STR_SIZE];
	char str_alpha[STR_SIZE];

	if( setValue(line, "file", str_file, STR_SIZE) != 0 )return;
	if( setValue(line, "name", str_name, STR_SIZE) != 0 )return;
	if( setValue(line, "alpha", str_alpha, STR_SIZE) != 0 )return;

	addImageData(str_file, isYesOrNO(str_alpha), str_name, IMAGE_GROUP_USER);
}

static void cmd_loadMusic(char *line)
{
	char str_file[STR_SIZE];
	char str_name[STR_SIZE];

	if( setValue(line, "file", str_file, STR_SIZE) != 0 )return;
	if( setValue(line, "name", str_name, STR_SIZE) != 0 )return;

	addMusic(str_file, str_name, MUSIC_GROUP_USER);
}

static void cmd_background(arena_t *arena, char *line)
{
	char str_image[STR_SIZE];

	if( setValue(line, "image", str_image, STR_SIZE) != 0 )return;

	arena->background = getImage(IMAGE_GROUP_USER, str_image);
}

static void cmd_playMusic(arena_t *arena, char *line)
{
	char str_music[STR_SIZE];

	if( setValue(line, "music", str_music, STR_SIZE) != 0 )return;

	strcpy(arena->music, str_music);
}

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

	if( setValue(line, "x", str_x, STR_NUM_SIZE) != 0 )return;
	if( setValue(line, "y", str_y, STR_NUM_SIZE) != 0 )return;
	if( setValue(line, "w", str_w, STR_NUM_SIZE) != 0 )return;
	if( setValue(line, "h", str_h, STR_NUM_SIZE) != 0 )return;
	if( setValue(line, "img_x", str_img_x, STR_NUM_SIZE) != 0 )return;
	if( setValue(line, "img_y", str_img_y, STR_NUM_SIZE) != 0 )return;
	if( setValue(line, "layer", str_layer, STR_NUM_SIZE) != 0 )return;
	if( setValue(line, "image", str_image, STR_SIZE) != 0 )return;

	new = newWall(atoi(str_x), atoi(str_y),
			atoi(str_w), atoi(str_h),
			atoi(str_img_x), atoi(str_img_y),
			atoi(str_layer), getImage(IMAGE_GROUP_USER, str_image) );

	addList(arena->listWall, new);
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

		if( strncmp(line, "loadImage", 9) == 0 )cmd_loadImage(line);
		if( strncmp(line, "loadMusic", 9) == 0 )cmd_loadMusic(line);
		if( strncmp(line, "background", 10) == 0 )cmd_background(arena, line);
		if( strncmp(line, "playMusic", 9) == 0 )cmd_playMusic(arena, line);
		if( strncmp(line, "wall", 4) == 0 )cmd_wall(arena, line);
	}

	return arena;
}

void initArenaFile()
{
	director_t *p;
	int i;

	assert( isImageInicialized() == TRUE );

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

