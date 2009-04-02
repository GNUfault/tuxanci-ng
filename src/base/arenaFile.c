
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
#include "archive.h"
#include "homeDirector.h"

#ifndef PUBLIC_SERVER
#    include "configFile.h"
#    include "image.h"
#endif

#ifndef NO_SOUND
#    include "music.h"
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

	if (offset_env == NULL)
		return -1;

	offset_val_begin = strchr(offset_env, '"');

	if (offset_val_begin == NULL)
		return -1;

	offset_val_end = strchr(offset_val_begin + 1, '"');

	if (offset_val_end == NULL)
		return -1;

	val_len = (int) (offset_val_end - (offset_val_begin + 1));

	if (val_len > len - 1)
		val_len = len - 1;

	memset(val, 0, len);
	memcpy(val, offset_val_begin + 1, val_len);

	return 0;
}


#ifndef PUBLIC_SERVER
image_t *loadImageFromArena(arenaFile_t * arenaFile, char *filename, char *group, char *name, int alpha)
{
	char *extractFile;
	image_t *image;

	extractFile = extractFileFromArchive(arenaFile->path, filename);
	image = addImageData(extractFile, alpha, name, group);
	deleteExtractFile(extractFile);

	return image;
}

#ifndef NO_SOUND
void loadMusicFromArena(arenaFile_t * arenaFile, char *filename, char *group, char *name)
{
	char *extractFile;

	extractFile = extractFileFromArchive(arenaFile->path, filename);
	addMusic(extractFile, name, group);
	deleteExtractFile(extractFile);
}
#endif
#endif

static void cmd_arena(arena_t ** arena, char *line)
{
	char str_image[STR_SIZE];
	char str_w[STR_SIZE];
	char str_h[STR_SIZE];

	if (getArenaValue(line, "background", str_image, STR_SIZE) != 0)
		return;

	if (getArenaValue(line, "w", str_w, STR_SIZE) != 0)
		return;

	if (getArenaValue(line, "h", str_h, STR_SIZE) != 0)
		return;

	(*arena) = newArena(atoi(str_w), atoi(str_h));
#ifndef PUBLIC_SERVER
	(*arena)->background = getImage(IMAGE_GROUP_USER, str_image);
#endif
	//(*arena)->w = atoi(str_w);
	//(*arena)->h = atoi(str_h);
	setCurrentArena(*arena);

	DEBUG_MSG(_("Loaded new arena...\n"));
}

static void cmd_loadModule(char *line)
{
	char str_file[STR_SIZE];

	if (getArenaValue(line, "file", str_file, STR_SIZE) != 0)
		return;

	loadModule(str_file);
}

#ifndef PUBLIC_SERVER

static void cmd_loadImage(arenaFile_t * arenaFile, char *line)
{
	char str_file[STR_SIZE];
	char str_name[STR_SIZE];
	char str_alpha[STR_SIZE];

	if (getArenaValue(line, "file", str_file, STR_SIZE) != 0)
		return;

	if (getArenaValue(line, "name", str_name, STR_SIZE) != 0)
		return;

	if (getArenaValue(line, "alpha", str_alpha, STR_SIZE) != 0)
		return;

	//printf("str_file=%s str_name=%s str_alpha=%s\n", str_file, str_name, str_alpha);
	loadImageFromArena(arenaFile, str_file, IMAGE_GROUP_USER, str_name, isYesOrNO(str_alpha));

	//addImageData(str_file, isYesOrNO(str_alpha), str_name, IMAGE_GROUP_USER);
}

static void cmd_loadMusic(arenaFile_t * arenaFile, char *line)
{
	char str_file[STR_SIZE];
	char str_name[STR_SIZE];

	if (getArenaValue(line, "file", str_file, STR_SIZE) != 0)
		return;

	if (getArenaValue(line, "name", str_name, STR_SIZE) != 0)
		return;

#ifndef NO_SOUND
	//addMusic(str_file, str_name, MUSIC_GROUP_USER);
	loadMusicFromArena(arenaFile, str_file, MUSIC_GROUP_USER, str_name);
#endif
}

static void cmd_playMusic(arena_t * arena, char *line)
{
	char str_music[STR_SIZE];

	if (getArenaValue(line, "music", str_music, STR_SIZE) != 0)
		return;

	strcpy(arena->music, str_music);
}
#endif

int getArenaCount()
{
	return listArenaFile->count;
}

static char *getArenaStatus(textFile_t * ts, char *s)
{
	int len;
	int i;

	len = strlen(s);

	for (i = 0; i < ts->text->count; i++) {
		char *line;

		line = (char *) (ts->text->list[i]);

		if (strncmp(line, s, len) == 0)
			return line + len + 1;
	}

	return NULL;
}

char *getArenaName(arenaFile_t * arenaFile)
{
	char *ret;

	ret = getArenaStatus(arenaFile->map, "name");
	return (ret != NULL ? ret : "arena_no_name");
}

char *getArenaNetName(arenaFile_t * arenaFile)
{
	char *ret;

	ret = getArenaStatus(arenaFile->map, "netName");
	return (ret != NULL ? ret : "arena_no_net_name");
}

arenaFile_t *getArenaFileFormNetName(char *s)
{
	int i;

	for (i = 0; i < listArenaFile->count; i++) {
		arenaFile_t *arenaFile;

		arenaFile = (arenaFile_t *) listArenaFile->list[i];

		if (strcmp(getArenaNetName(arenaFile), s) == 0)
			return arenaFile;
	}

	return NULL;
}

char *getArenaImage(arenaFile_t * arenaFile)
{
	return getArenaStatus(arenaFile->map, "screen");
}

arenaFile_t *getArenaFile(int n)
{
	return (arenaFile_t *) listArenaFile->list[n];
}

int getArenaFileID(arenaFile_t * arenaFile)
{
	int i;

	for (i = 0; i < listArenaFile->count; i++) {
		arenaFile_t *this;

		this = (arenaFile_t *) listArenaFile->list[i];

		if (this == arenaFile) {
			return i;
		}
	}

	return -1;
}

arena_t *getArena(arenaFile_t * arenaFile)
{
	textFile_t *ts;
	arena_t *arena = NULL;		// no warninng
	int i;

	ts = (textFile_t *) arenaFile->map;

	for (i = 0; i < ts->text->count; i++) {
		char *line;

		line = (char *) (ts->text->list[i]);
		cmdModule(line);
#ifndef PUBLIC_SERVER
		if (strncmp(line, "loadImage", 9) == 0)
			cmd_loadImage(arenaFile, line);

		if (strncmp(line, "loadMusic", 9) == 0)
			cmd_loadMusic(arenaFile, line);

		if (strncmp(line, "playMusic", 9) == 0)
			cmd_playMusic(arena, line);

#endif
		if (strncmp(line, "arena", 5) == 0)
			cmd_arena(&arena, line);

		if (strncmp(line, "loadModule", 10) == 0)
			cmd_loadModule(line);
	}

	return arena;
}

arenaFile_t *newArenaFile(char *path)
{
	arenaFile_t *new;
	char *extractFile;

	new = malloc(sizeof(arenaFile_t));
	new->path = strdup(path);

	extractFile = extractFileFromArchive(path, "arena.map");
	new->map = loadTextFile(extractFile);
	deleteExtractFile(extractFile);

	return new;
}

void destroyArenaFile(arenaFile_t * p)
{
	destroyTextFile(p->map);
	free(p->path);
	free(p);
}

void loadArenaFile(char *path)
{
	addList(listArenaFile, newArenaFile(path));
}

static void loadArenaFromDirector(char *director)
{
	director_t *p;
	int i;

	DEBUG_MSG(_("Loading arena files from %s\n"), director);
	p = loadDirector(director);

	if (p==NULL)
	{
		fprintf(stderr, _("Directory %s not found !\n"), director);
		exit(-1);
	}

	for (i = 0; i < p->list->count; i++) {
		char *line;

		line = (char *) (p->list->list[i]);

		if (strstr(line, ".zip") != NULL && strstr(line, "~")==NULL) {
			char path[STR_PATH_SIZE];

			if (director[strlen(director)-1]=='/') {
				sprintf(path, "%s%s", director, line);
			}
			else {
				sprintf(path, "%s/%s", director, line);
			}

			DEBUG_MSG(_("Loading arena: %s\n"), line);

			accessExistFile(path);
			loadArenaFile(path);
		}
	}

	//printf("No. of Arens: %d\n", listArenaFile->count);
	destroyDirector(p);

}

void initArenaFile()
{
#ifndef PUBLIC_SERVER
	assert(isImageInicialized() == TRUE);
#endif
	isArenaFileInit = TRUE;
	listArenaFile = newList();

	loadArenaFromDirector(PATH_ARENA);

#ifndef PUBLIC_SERVER
	loadArenaFromDirector(getHomeDirector());
#endif
}

void quitArenaFile()
{
	isArenaFileInit = FALSE;
	destroyListItem(listArenaFile, destroyArenaFile);
}
