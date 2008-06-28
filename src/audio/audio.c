
#include <stdio.h>
#include <assert.h>
#include <SDL_mixer.h>

#include "base/main.h"
#include "audio/audio.h"
#include "client/interface.h"

static bool_t isAudioInit = FALSE;

bool_t isAudioInicialized()
{
	return isAudioInit;
}

void initAudio()
{
	if( SDL_Init(SDL_INIT_AUDIO) == -1 )
	{
		fprintf(stderr, "Nelze inicializovat audio : %s\n", SDL_GetError());
		return;
	}

	if( Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 1024) == -1 )
	{
        	fprintf(stderr, "Nelze inicializovat audio : %s\n", Mix_GetError());
        	return;
	}

	Mix_AllocateChannels(16);
	Mix_Volume(-1, MIX_MAX_VOLUME);

	printf("init audio..\n");
	isAudioInit = TRUE;
}

void quitAudio()
{
	Mix_CloseAudio();
	isAudioInit = TRUE;
	printf("quit audio..\n");
}

