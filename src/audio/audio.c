#include <stdio.h>
#include <assert.h>
#include <SDL_mixer.h>

#include "main.h"
#include "audio.h"
#include "interface.h"

static bool_t isAudioInit = FALSE;

/*
 * Return status of audio
 */
bool_t audio_is_inicialized()
{
	return isAudioInit;
}

/*
 * Initialize audio
 */
void audio_init()
{
	if (isParamFlag("--no-audio")) {
		return;
	}

	DEBUG_MSG(_("[Debug] Initializing audio\n"));

	if (SDL_InitSubSystem(SDL_INIT_AUDIO) == -1) {
		fprintf(stderr, _("[Error] Unable to initialize audio: %s\n"), SDL_GetError());
		return;
	}

	if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 1024) == -1) {
		fprintf(stderr, _("[Error] Unable to create proper audio settings: %s\n"), Mix_GetError());
		return;
	}

	Mix_AllocateChannels(16);
	Mix_Volume(-1, MIX_MAX_VOLUME);

	isAudioInit = TRUE;
}

/*
 * Shutdown audio
 */
void audio_quit()
{
	DEBUG_MSG(_("[Debug] Shutting down audio\n"));

	Mix_CloseAudio();
	isAudioInit = TRUE;
}
