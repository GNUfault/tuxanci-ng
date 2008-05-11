
#ifndef SOUND_H

#define SOUND_H

#ifndef NO_SOUND

#include <SDL_mixer.h>
#include "main.h"

#define SOUND_GROUP_BASE 0
#define SOUND_GROUP_USER 1

typedef struct sound_struct
{
	char *name;
	int group;
	Mix_Chunk *sound;
} sound_t;

extern bool_t isSoundInicialized();
extern void initSound();
extern void addSound(char *file, char *name, int group);
extern void playSound(char *name, int group);
extern void setSoundActive(bool_t n);
extern bool_t isSoundActive();
extern void quitSound();

#endif

#endif
