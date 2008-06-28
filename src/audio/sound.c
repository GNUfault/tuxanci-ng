
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "base/main.h"
#include "base/list.h"
#include "base/storage.h"

#include "client/interface.h"

#include "audio/audio.h"
#include "audio/sound.h"

static list_t *listStorage;

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
		return;
	}

	listStorage = newStorage();
	isSoundInit = TRUE;
	var_isSoundActive = TRUE;

	if( isParamFlag("--nosound") )
	{
		setSoundActive(FALSE);
	}

	if( isParamFlag("--sound") )
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

	accessExistFile(str);

	new = Mix_LoadWAV(str);

	if( new == NULL )
	{
		fprintf(stderr, "Nelze nahrat zvuk %s : %s\n", str, Mix_GetError());
		return NULL;
	}

	return new;
}


static void playMixSound(Mix_Chunk *p)
{
	if( Mix_PlayChannel(-1, p, 0) == -1 )
	{
		fprintf(stderr, "Nelze prehrat zvuk : %s\n", Mix_GetError());
		return;
	}
}

static void destroySound(void *p)
{
	Mix_FreeChunk((Mix_Chunk *)p);
}

void addSound(char *file, char *name, char *group)
{
	Mix_Chunk *new;

	if( isSoundInit == FALSE )
	{
		return;
	}

	assert( file != NULL );
	assert( name != NULL );
	assert( group != NULL );

	new = loadMixSound(file);

	addItemToStorage(listStorage, group, name, new );
}

void playSound(char *name, char *group)
{
	if( isSoundInit == FALSE ||
	    var_isSoundActive == FALSE )
	{
		return;
	}

	playMixSound( getItemFromStorage(listStorage, group, name) );
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

	destroyStorage(listStorage, destroySound);
	isSoundInit = FALSE;
	printf("quit sound..\n");
}

