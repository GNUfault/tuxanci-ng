#include <stdio.h>
#include <assert.h>
#include <SDL2/SDL_mixer.h>

#include "main.h"
#include "audio.h"
#include "interface.h"

static bool_t isAudioInit = FALSE;

/**
 * Return status of audio
 */
bool_t audio_is_inicialized()
{
	return isAudioInit;
}

/**
 * Initialize audio
 */
void audio_init()
{
	if (isParamFlag("--no-audio")) {
		return;
	}

	debug("Initializing audio");

	if (SDL_InitSubSystem(SDL_INIT_AUDIO) == -1) {
		error("Unable to initialize audio: %s", SDL_GetError());
		return;
	}

	if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 1024) == -1) {
		error("Unable to create proper audio settings: %s", Mix_GetError());
		return;
	}

	Mix_AllocateChannels(16);
	Mix_Volume(-1, MIX_MAX_VOLUME);

	isAudioInit = TRUE;
}

/**
 * Shutdown audio
 */
void audio_quit()
{
	debug("Shutting down audio");

	Mix_CloseAudio();
	isAudioInit = TRUE;
}
