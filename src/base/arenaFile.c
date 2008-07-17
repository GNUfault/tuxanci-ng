
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
#include "item.h"
#include "director.h"
#include "modules.h"

#ifndef PUBLIC_SERVER
#include "configFile.h"
#include "image.h"
#endif
#ifndef NO_SOUND
#include "music.h"
#endif

static list_t *listArenaFile;

static bool_t isArenaFileInit = FALSE;

bool_t isAreaFileInicialized()
{
	return isArenaFileInit;
}

int getArenaValue(char *line, char *env, char *val, int len)
{
	char *offset_env;
	char *offset_val_begin;
	char *offset_val_end;
	char clone_env[STR_SIZE];
	int val_len;

	strcpy(clone_env, " ");
	strcat(clone_env, env);

	offset_env = strstr(line, clone_env);
	if( offset_env == NULL )return -1;
	
	offset_val_begin = strchr(offset_env, '"');
	if( offset_val_begin == NULL )return -1;
	
	offset_val_end = strchr(offset_val_begin+1, '"');
	if( offset_val_end == NULL )return -1;
	
	val_len = (int)(offset_val_end - ( offset_val_begin + 1) );
	if( val_len > len - 1 ) val_len = len - 1;

	memset(val, 0, len);
	memcpy(val, offset_val_begin+1, val_len);

	return 0;
}


static void cmd_arena(arena_t **arena, char *line)
{
	char str_image[STR_SIZE];
	char str_w[STR_SIZE];
	char str_h[STR_SIZE];

	if( getArenaValue(line, "background", str_image, STR_SIZE) != 0 )return;
	if( getArenaValue(line, "w", str_w, STR_SIZE) != 0 )return;
	if( getArenaValue(line, "h", str_h, STR_SIZE) != 0 )return;

	(*arena) = newArena(atoi(str_w), atoi(str_h));
#ifndef PUBLIC_SERVER
	(*arena)->background = getImage(IMAGE_GROUP_USER, str_image);
#endif
	//(*arena)->w = atoi(str_w);
	//(*arena)->h = atoi(str_h);
	setCurrentArena(*arena);
	printf("new arena\n");
}

static void cmd_loadModule(char *line)
{
	char str_file[STR_SIZE];

	if( getArenaValue(line, "file", str_file, STR_SIZE) != 0 )return;

	loadModule(str_file);
}

#ifndef PUBLIC_SERVER

static void cmd_loadImage(char *line)
{
	char str_file[STR_SIZE];
	char str_name[STR_SIZE];
	char str_alpha[STR_SIZE];

	if( getArenaValue(line, "file", str_file, STR_SIZE) != 0 )return;
	if( getArenaValue(line, "name", str_name, STR_SIZE) != 0 )return;
	if( getArenaValue(line, "alpha", str_alpha, STR_SIZE) != 0 )return;

	addImageData(str_file, isYesOrNO(str_alpha), str_name, IMAGE_GROUP_USER);
}

static void cmd_loadMusic(char *line)
{
	char str_file[STR_SIZE];
	char str_name[STR_SIZE];

	if( getArenaValue(line, "file", str_file, STR_SIZE) != 0 )return;
	if( getArenaValue(line, "name", str_name, STR_SIZE) != 0 )return;

#ifndef NO_SOUND
	addMusic(str_file, str_name, MUSIC_GROUP_USER);
#endif
}

static void cmd_playMusic(arena_t *arena, char *line)
{
	char str_music[STR_SIZE];

	if( getArenaValue(line, "music", str_music, STR_SIZE) != 0 )return;

	strcpy(arena->music, str_music);
}

#endif

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
	arena_t *arena = NULL;  // no warninng
	int i;

	ts = (textFile_t *) listArenaFile->list[id];

	for( i = 0 ; i < ts->text->count ; i++ )
	{
		char *line;
		line = (char *) (ts->text->list[i]);

		cmdModule(line);

#ifndef PUBLIC_SERVER
		if( strncmp(line, "loadImage", 9) == 0 )cmd_loadImage(line);
		if( strncmp(line, "loadMusic", 9) == 0 )cmd_loadMusic(line);
		if( strncmp(line, "playMusic", 9) == 0 )cmd_playMusic(arena, line);
#endif

		if( strncmp(line, "arena", 5) == 0 )cmd_arena(&arena, line);
		if( strncmp(line, "loadModule", 10) == 0 )cmd_loadModule(line);
		//if( strncmp(line, "wall", 4) == 0 )cmd_wall(arena, line);
		//if( strncmp(line, "teleport", 8) == 0 )cmd_teleport(arena, line);
		//if( strncmp(line, "pipe", 4) == 0 )cmd_pipe(arena, line);
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

	printf("pocet aren: %d\n", listArenaFile->count);

	destroyDirector(p);
}

void quitArenaFile()
{
	isArenaFileInit = FALSE;
	destroyListItem(listArenaFile, destroyTextFile);
}

