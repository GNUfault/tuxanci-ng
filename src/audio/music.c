
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "base/main.h"
#include "base/list.h"
#include "base/storage.h"

#include "client/interface.h"

#include "audio/audio.h"
#include "audio/music.h"

static list_t *listStorage;

static bool_t isMusicInit = FALSE;
static bool_t var_isMusicActive = TRUE;
static Mix_Music *currentMusic;

bool_t isMusicInicialized()
{
	return isMusicInit;
}

void initMusic()
{
	if( isAudioInicialized() == FALSE )
	{
		isMusicInit = FALSE;
		return;
	}

	listStorage = newStorage();
	currentMusic = NULL;

	isMusicInit = TRUE;
	var_isMusicActive = TRUE;

	if( isParamFlag("--nomusic") )
	{
		setMusicActive(FALSE);
	}

	if( isParamFlag("--music") )
	{
		setMusicActive(TRUE);
	}

	printf("init music..\n");
}

static Mix_Music* loadMixMusic(char *file)
{
	Mix_Music *new;
	char str[STR_PATH_SIZE];


	printf("load music %s..\n", file);
	sprintf(str, PATH_MUSIC "%s", file);

	accessExistFile(str);

	new = Mix_LoadMUS(str);

	if( new == NULL )
	{
		fprintf(stderr, "Nepodarilo sa nacitat hudbu %s\n", file);
		return NULL;
	}

	return new;
}

static void playMixMusic()
{
	if( currentMusic != NULL )
	{
		Mix_PlayMusic(currentMusic, 1000);
	}
}

static void destroyMusic(void *p)
{
	Mix_FreeMusic((Mix_Music *)p);
}

void addMusic(char *file, char *name, char *group)
{
	Mix_Music *new;

	if( isMusicInit == FALSE )
	{
		return;
	}

	assert( file != NULL );
	assert( name != NULL );
	assert( group != NULL );

	new = loadMixMusic(file);

	addItemToStorage(listStorage, group, name, new );
}

void stopMusic()
{
	if( isMusicInit == FALSE ||
	    var_isMusicActive == FALSE )
	{
		return;
	}

	if( currentMusic != NULL )
	{
		printf("stop music..\n");
		Mix_HaltMusic();
		currentMusic = NULL;
	}
}

void playMusic(char *name, char *group)
{
	static char currentMusic_group[STR_SIZE];
	static char currentMusic_name[STR_SIZE];
	static int isStrInit = 0;

	if( isStrInit == 0 )
	{
		strcpy(currentMusic_group, "none");
		strcpy(currentMusic_name, "none");
		isStrInit = 1;	
	}

	if( isMusicInit == FALSE ||
	    var_isMusicActive == FALSE )
	{
		return;
	}

	if( currentMusic != NULL &&
	    strcmp(currentMusic_group, group) == 0 &&
	    strcmp(currentMusic_name, name) == 0 )
	{
		return;
	}

	if( currentMusic != NULL )
	{
		stopMusic();
	}

	currentMusic = getItemFromStorage(listStorage, group, name);
	strcpy(currentMusic_group, group);
	strcpy(currentMusic_name, name);

	if( currentMusic != NULL )
	{
		printf("play music %s\n", name);
		playMixMusic();
	}
}

void setMusicActive(bool_t n)
{
	static Mix_Music *music = NULL;

	if( n == FALSE )
	{
		music = currentMusic;
		stopMusic();
	}

	if( n == TRUE )
	{
		currentMusic = music;
		playMixMusic();
	}

	var_isMusicActive = n;
}

bool_t isMusicActive()
{
	return var_isMusicActive;
}

char* getCurrentMusic()
{
	return "unknown";
}

void delAllMusicInGroup(char *group)
{
	if( isMusicInit == FALSE )
	{
		return;
	}

	delAllItemFromStorage(listStorage, group, destroyMusic);
}

void quitMusic()
{
	if( isMusicInit == FALSE )
	{
		return;
	}

	stopMusic();
	destroyStorage(listStorage, destroyMusic);
	isMusicInit = FALSE;

	printf("quit music..\n");
}
