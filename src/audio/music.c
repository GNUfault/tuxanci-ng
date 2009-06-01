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
static bool_t var_music_is_active = TRUE;
static Mix_Music *currentMusic;

/*
 * Return status of music
 */
bool_t music_is_inicialized()
{
	return isMusicInit;
}

/*
 * Initialize music
 */
void music_init()
{
	if (audio_is_inicialized() == FALSE) {
		isMusicInit = FALSE;
		return;
	}

	DEBUG_MSG(_("[Debug] Initializing music\n"));

	listStorage = storage_new();
	currentMusic = NULL;
	isMusicInit = TRUE;
	var_music_is_active = TRUE;

	if (isParamFlag("--no-music")) {
		music_set_active(FALSE);
	} else if (isParamFlag("--music")) {
		music_set_active(TRUE);
	}
}

/*
 * Load a file with music and returns it
 */
static Mix_Music *loadMixMusic(char *file)
{
	Mix_Music *mixer;
	char str[STR_PATH_SIZE];

	DEBUG_MSG(_("[Debug] Loading music [%s]\n"), file);

	if (isFillPath(file)) {
		strcpy(str, file);
	} else {
		sprintf(str, PATH_MUSIC "%s", file);
	}

	accessExistFile(str);

	mixer = Mix_LoadMUS(str);

	if (mixer == NULL) {
		fprintf(stderr, _("[Error] Unable to load music [%s]\n"), file);
		return NULL;
	}

	return mixer;
}

/*
 * Prepare the music mixer
 */
static void playMixMusic()
{
	if (currentMusic != NULL) {
		Mix_PlayMusic(currentMusic, 1000);
	}
}

/*
 * Remove music from the memory
 */
static void destroyMusic(void *p)
{
	Mix_FreeMusic((Mix_Music *) p);
}

/*
 * Add a file to the music list
 */
void music_add(char *file, char *name, char *group)
{
	Mix_Music *new;

	if (isMusicInit == FALSE) {
		return;
	}

	assert(file != NULL);
	assert(name != NULL);
	assert(group != NULL);

	new = loadMixMusic(file);
	storage_add(listStorage, group, name, new);
}

/*
 * Stop playing music
 */
void music_stop()
{
	if (isMusicInit == FALSE || var_music_is_active == FALSE) {
		return;
	}

	if (currentMusic != NULL) {
		DEBUG_MSG(_("[Debug] Stopping playing music\n"));

		Mix_HaltMusic();
		currentMusic = NULL;
	}
}

/*
 * Play music playback
 */
void music_play(char *name, char *group)
{
	static char currentMusic_group[STR_SIZE];
	static char currentMusic_name[STR_SIZE];
	static int isStrInit = 0;

	if (isStrInit == 0) {
		strcpy(currentMusic_group, "none");
		strcpy(currentMusic_name, "none");
		isStrInit = 1;
	}

	if (isMusicInit == FALSE || var_music_is_active == FALSE) {
		return;
	}

	if (currentMusic != NULL &&
	    strcmp(currentMusic_group, group) == 0 &&
	    strcmp(currentMusic_name, name) == 0) {
		return;
	}

	if (currentMusic != NULL) {
		music_stop();
	}

	currentMusic = storage_get(listStorage, group, name);
	strcpy(currentMusic_group, group);
	strcpy(currentMusic_name, name);

	if (currentMusic != NULL) {
		DEBUG_MSG(_("[Debug] Playing music [%s]\n"), name);

		playMixMusic();
	}
}

/*
 * Set music status to active/inactive
 */
void music_set_active(bool_t n)
{
	static Mix_Music *music = NULL;

	if (n == FALSE) {
		music = currentMusic;
		music_stop();
	} else if (n == TRUE) {
		currentMusic = music;
		playMixMusic();
	}

	var_music_is_active = n;
}

/*
 * Return the activity status of music
 */
bool_t music_is_active()
{
	return var_music_is_active;
}

/*
 * Return the music playing right now
 */
char *music_get_current()
{
	return "unknown";
}

/*
 * Remove all music files belonging to the certain group
 */
void music_del_all_in_group(char *group)
{
	if (isMusicInit == FALSE) {
		return;
	}

	storage_del_all(listStorage, group, destroyMusic);
}

/*
 * Shutdown music
 */
void music_quit()
{
	if (isMusicInit == FALSE) {
		return;
	}

	DEBUG_MSG(_("[Debug] Shutting down music\n"));

	music_stop();
	storage_destroy(listStorage, destroyMusic);
	isMusicInit = FALSE;
}
