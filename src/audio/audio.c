
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
bool_t
isAudioInicialized()
{
    return isAudioInit;
}

/*
 * Inicialize audio
 */
void
initAudio()
{
    if (isParamFlag("--noaudio")) {
        return;
    }

    if (SDL_Init(SDL_INIT_AUDIO) == -1) {
        fprintf(stderr, _("Unable to initialize audio: %s\n"),
                SDL_GetError());
        return;
    }
    if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 1024) ==
        -1) {
        fprintf(stderr, _("Unable to create proper audio settings: %s\n"),
                Mix_GetError());
        return;
    }
    Mix_AllocateChannels(16);
    Mix_Volume(-1, MIX_MAX_VOLUME);
#ifdef DEBUG
    printf(_("Initializing audio system...\n"));
#endif
    isAudioInit = TRUE;
}

/*
 * Quit audio
 */
void
quitAudio()
{
    Mix_CloseAudio();
    isAudioInit = TRUE;
#ifdef DEBUG
    printf(_("Quitting audio...\n"));
#endif
}
