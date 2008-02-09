
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "main.h"
#include "interface.h"
#include "list.h"
#include "path.h"
#include "string_length.h"
#include "audio.h"
#include "sound.h"

static list_t *listSound;

static bool_t isSoundInit = FALSE;
static bool_t var_isSoundActive = TRUE;

bool_t isSoundInicialized()
{
	return isSoundInit;
}

void initSound()
{
	if( isAudioInicialized() == FALSE )
	{
		isSoundInit = FALSE;
	}

	listSound = newList();
	isSoundInit = TRUE;
	var_isSoundActive = TRUE;

	if( isParamFlag("--nomusic") )
	{
		setSoundActive(FALSE);
	}

	if( isParamFlag("--music") )
	{
		setSoundActive(TRUE);
	}

	printf("init sound..\n");
}

static Mix_Chunk* loadMixSound(char *file)
{
	Mix_Chunk *new;
	char str[STR_PATH_SIZE];

	printf("load sound %s..\n", file);
	sprintf(str, PATH_SOUND "%s", file);
	new = Mix_LoadWAV(str);

	if( new == NULL )
	{
		fprintf(stderr, "Nelze nahrat zvuk %s : %s\n", str, Mix_GetError());
		return NULL;
	}

	return new;
}

static sound_t *newSound(char *file, char *name, int group)
{
	sound_t *new;

	new = malloc( sizeof(sound_t) );
	new->name = strdup(name);
	new->group = group;
	new->sound = loadMixSound(file);

	return new;
}

static void playMixSound(sound_t *p)
{
	if( Mix_PlayChannel(-1, p->sound, 0) == -1 )
	{
		fprintf(stderr, "Nelze prehrat zvuk : %s\n", Mix_GetError());
		return;
	}
}

static void destroySound(sound_t *p)
{
	free(p->name);
	Mix_FreeChunk(p->sound);
	free(p);
}

void addSound(char *file, char *name, int group)
{
	addList( listSound, newSound(file, name, group) );
}

void playSound(char *name, int group)
{
	int i;
	sound_t *this;

	if( isSoundInit == FALSE ||
	    var_isSoundActive == FALSE )
	{
		return;
	}

	for( i = 0 ; i < listSound->count ; i++ )
	{
		this = (sound_t *)listSound->list[i];

		if( this->group == group && strcmp(this->name, name) == 0 )
		{
			playMixSound(this);
			return;
		}
	}
}

void setSoundActive(bool_t n)
{
	var_isSoundActive = n;
}

bool_t isSoundActive()
{
	return var_isSoundActive;
}

void quitSound()
{
	if( isSoundInit == FALSE )
	{
		return;
	}

	destroyListItem(listSound, destroySound);
	isSoundInit = FALSE;
	printf("quit sound..\n");
}

