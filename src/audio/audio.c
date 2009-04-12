
#include <stdio.h>
#include <assert.h>
#include <SDL_mixer.h>

#include "main.h"
#include "audio.h"
#include "interface.h"

static bool_t isAudioInit = FALSE;

/*
 * Returns state of audio
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

	if (SDL_Init(SDL_INIT_AUDIO) == -1) {
		fprintf(stderr, _("Error! Unable to initialize audio: %s\n"), SDL_GetError());
		return;
	}

	if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 1024) == -1)
	{
		fprintf(stderr, _("Unable to create proper audio settings: %s\n"),
				Mix_GetError());
		return;
	}

	Mix_AllocateChannels(16);
	Mix_Volume(-1, MIX_MAX_VOLUME);

	DEBUG_MSG(_("Initializing audio\n"));

	isAudioInit = TRUE;
}

/*
 * Quit audio
 */
void audio_quit()
{
	Mix_CloseAudio();
	isAudioInit = TRUE;

	DEBUG_MSG(_("Quitting audio\n"));
}
