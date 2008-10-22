
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
static bool_t var_isSoundActive = TRUE;
/*
 * Return state of sound
 */
bool_t isSoundInicialized()
{
	return isSoundInit;
}
/*
 * Audio initialization
 */
void initSound()
{
	if( isAudioInicialized() == FALSE ) {
		isSoundInit = FALSE;
		return;
	}

	listStorage = newStorage();
	isSoundInit = TRUE;
	var_isSoundActive = TRUE;
	if (isParamFlag("--nosound"))
		setSoundActive(FALSE);
	if (isParamFlag("--sound"))
		setSoundActive(TRUE);
#ifdef DEBUG
	printf(_("Initializing sound...\n"));
#endif
}
/*
 * Load sound file (Null/mixer)
 */
static Mix_Chunk* loadMixSound(char *file)
{
	Mix_Chunk *new;
	char str[STR_PATH_SIZE];

#ifdef DEBUG
	printf(_("Loading sound file: %s\n"), file);
#endif
	sprintf(str, PATH_SOUND "%s", file);
	accessExistFile(str);
	new = Mix_LoadWAV(str);
	if (new == NULL) {
		fprintf(stderr, _("Unable to load sound file: %s with error: %s\n"), str, Mix_GetError());
		return NULL;
	}
	return new;
}

/*
 * Play sound with mixer
 */
static void playMixSound(Mix_Chunk *p)
{
	if (Mix_PlayChannel(-1, p, 0) == -1) {
		fprintf(stderr, _("Unable to playback sound with error: %s\n"), Mix_GetError());
		return;
	}
}
/*
 * Destroy mixer chunk
 */
static void destroySound(void *p)
{
	Mix_FreeChunk((Mix_Chunk *)p);
}
/*
 * Add sound to list
 */
void addSound(char *file, char *name, char *group)
{
	Mix_Chunk *new;

	if (isSoundInit == FALSE)
		return;
	assert( file != NULL );
	assert( name != NULL );
	assert( group != NULL );
	new = loadMixSound(file);
	addItemToStorage(listStorage, group, name, new );
}
/*
 * Start playback file from list
 */
void playSound(char *name, char *group)
{
	if (isSoundInit == FALSE ||
			var_isSoundActive == FALSE)
		return;
	playMixSound(getItemFromStorage(listStorage, group, name));
}
/*
 * Set if sound is active True/False
 */
void setSoundActive(bool_t n)
{
	var_isSoundActive = n;
}
/*
 * Return state of sound
 */
bool_t isSoundActive()
{
	return var_isSoundActive;
}
/*
 * Quit all sound stuff
 */
void quitSound()
{
	if( isSoundInit == FALSE )
		return;
	destroyStorage(listStorage, destroySound);
	isSoundInit = FALSE;
#ifdef DEBUG
	printf(_("Quitting sound...\n"));
#endif
}

