#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "main.h"
#include "list.h"
#include "storage.h"
#include "interface.h"
#include "audio.h"
#include "sound.h"

static list_t *listStorage;
static bool_t isSoundInit = FALSE;
static bool_t var_sound_is_active = TRUE;

/**
 * Return status of sound
 */
bool_t sound_is_inicialized()
{
	return isSoundInit;
}

/**
 * Initialize sound
 */
void sound_init()
{
	if (audio_is_inicialized() == FALSE) {
		isSoundInit = FALSE;
		return;
	}

	DEBUG_MSG(_("[Debug] Initializing sound\n"));

	listStorage = storage_new();
	isSoundInit = TRUE;
	var_sound_is_active = TRUE;

	if (isParamFlag("--no-sound")) {
		sound_set_active(FALSE);
	} else if (isParamFlag("--sound")) {
		sound_set_active(TRUE);
	}
}

/**
 * Load sound from file (Null/mixer)
 */
static Mix_Chunk *loadMixSound(char *file)
{
	Mix_Chunk *new;
	char str[STR_PATH_SIZE];

	DEBUG_MSG(_("[Debug] Loading sound [%s]\n"), file);

	sprintf(str, PATH_SOUND "%s", file);
	accessExistFile(str);
	new = Mix_LoadWAV(str);

	if (new == NULL) {
		fprintf(stderr, _("[Error] Unable to load sound [%s]: %s\n"), str, Mix_GetError());
		return NULL;
	}

	return new;
}

/**
 * Play sound with mixer
 */
static void playMixSound(Mix_Chunk *p)
{
	if (Mix_PlayChannel(-1, p, 0) == -1) {
		fprintf(stderr, _("[Error] Unable to play sound: %s\n"), Mix_GetError());
		return;
	}
}

/**
 * Destroy mixer chunk
 */
static void destroySound(void *p)
{
	Mix_FreeChunk((Mix_Chunk *) p);
}

/**
 * Add a sound to list
 */
void sound_add(char *file, char *name, char *group)
{
	Mix_Chunk *new;

	if (isSoundInit == FALSE) {
		return;
	}

	assert(file != NULL);
	assert(name != NULL);
	assert(group != NULL);

	new = loadMixSound(file);
	storage_add(listStorage, group, name, new);
}

/**
 * Start playing a sound from a file from the list
 */
void sound_play(char *name, char *group)
{
	if (isSoundInit == FALSE || var_sound_is_active == FALSE) {
		return;
	}

	playMixSound(storage_get(listStorage, group, name));
}

/**
 * Set sound status to active/inactive (true/false)
 */
void sound_set_active(bool_t n)
{
	var_sound_is_active = n;
}

/**
 * Return status of sound
 */
bool_t sound_is_active()
{
	return var_sound_is_active;
}

/**
 * Shutdown sound
 */
void sound_quit()
{
	if (isSoundInit == FALSE) {
		return;
	}

	DEBUG_MSG(_("[Debug] Shutting down sound\n"));

	storage_destroy(listStorage, destroySound);
	isSoundInit = FALSE;
}
