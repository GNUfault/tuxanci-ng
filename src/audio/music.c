
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "main.h"
#include "list.h"
#include "storage.h"
#include "interface.h"
#include "audio.h"
#include "music.h"

static list_t *listStorage;

static bool_t isMusicInit = FALSE;
static bool_t var_isMusicActive = TRUE;
static Mix_Music *currentMusic;

/*
 * Return state of music initialization
 */
bool_t isMusicInicialized()
{
	return isMusicInit;
}

/*
 * Initialize music
 */
void initMusic()
{
	if (isAudioInicialized() == FALSE) {
		isMusicInit = FALSE;
		return;
	}

	listStorage = newStorage();
	currentMusic = NULL;
	isMusicInit = TRUE;
	var_isMusicActive = TRUE;

	if (isParamFlag("--no-music"))
		setMusicActive(FALSE);

	if (isParamFlag("--music"))
		setMusicActive(TRUE);

	DEBUG_MSG(_("Initializing music\n"));
}

/*
 * Load a file with music and returns it
 */
static Mix_Music *loadMixMusic(char *file)
{
	Mix_Music *mixer;
	char str[STR_PATH_SIZE];

	DEBUG_MSG(_("Loading a file with music: %s\n"), file);

	if (isFillPath(file)) {
		strcpy(str, file);
	} else {
		sprintf(str, PATH_MUSIC "%s", file);
	}

	accessExistFile(str);

	mixer = Mix_LoadMUS(str);

	if (mixer == NULL) {
		fprintf(stderr, _("Unable to load music from file %s\n"), file);
		return NULL;
	}

	return mixer;
}

/*
 * Prepare music mixer
 */
static void playMixMusic()
{
	if (currentMusic != NULL) {
		Mix_PlayMusic(currentMusic, 1000);
	}
}

/*
 * Free memory of music
 */
static void destroyMusic(void *p)
{
	Mix_FreeMusic((Mix_Music *) p);
}

/*
 * Add file to list with music
 */
void addMusic(char *file, char *name, char *group)
{
	Mix_Music *new;

	if (isMusicInit == FALSE)
		return;

	assert(file != NULL);
	assert(name != NULL);
	assert(group != NULL);

	new = loadMixMusic(file);
	addItemToStorage(listStorage, group, name, new);
}

/*
 * Disable music and stop playback
 */
void stopMusic()
{
	if (isMusicInit == FALSE || var_isMusicActive == FALSE) {
		return;
	}

	if (currentMusic != NULL) {
		DEBUG_MSG(_("Stopping music\n"));

		Mix_HaltMusic();
		currentMusic = NULL;
	}
}

/*
 * Play music playback
 */
void playMusic(char *name, char *group)
{
	static char currentMusic_group[STR_SIZE];
	static char currentMusic_name[STR_SIZE];
	static int isStrInit = 0;

	if (isStrInit == 0) {
		strcpy(currentMusic_group, "none");
		strcpy(currentMusic_name, "none");
		isStrInit = 1;
	}

	if (isMusicInit == FALSE || var_isMusicActive == FALSE)
		return;

	if (currentMusic != NULL &&
	    strcmp(currentMusic_group, group) == 0 &&
	    strcmp(currentMusic_name, name) == 0) {
		return;
	}

	if (currentMusic != NULL) {
		stopMusic();
	}

	currentMusic = getItemFromStorage(listStorage, group, name);
	strcpy(currentMusic_group, group);
	strcpy(currentMusic_name, name);

	if (currentMusic != NULL) {
		DEBUG_MSG(_("Playing music from file %s\n"), name);

		playMixMusic();
	}
}

/*
 * Set music status to active/inactive
 */
void setMusicActive(bool_t n)
{
	static Mix_Music *music = NULL;

	if (n == FALSE) {
		music = currentMusic;
		stopMusic();
	}
	if (n == TRUE) {
		currentMusic = music;
		playMixMusic();
	}

	var_isMusicActive = n;
}

/*
 * Return status of music
 */
bool_t isMusicActive()
{
	return var_isMusicActive;
}

/*
 * TOCOMMENT
 */
char *getCurrentMusic()
{
	return "unknown";
}

/*
 * TOCOMMENT
 */
void delAllMusicInGroup(char *group)
{
	if (isMusicInit == FALSE)
		return;

	delAllItemFromStorage(listStorage, group, destroyMusic);
}

/*
 * Deactivate all what is ment for music
 */
void quitMusic()
{
	if (isMusicInit == FALSE)
		return;

	stopMusic();
	destroyStorage(listStorage, destroyMusic);
	isMusicInit = FALSE;

	DEBUG_MSG(_("Quitting music\n"));
}
