
#ifndef MUSIC_H

#define MUSIC_H

#ifndef NO_SOUND

#include <SDL_mixer.h>
#include "main.h"

#define MUSIC_GROUP_BASE 0
#define MUSIC_GROUP_USER 1

typedef struct music_struct
{
	char *name;
	int group;
	Mix_Music *music;
} music_t;

extern bool_t isMusicInicialized();
extern void initMusic();
extern void addMusic(char *file, char *name, int group);
extern void stopMusic();
extern void playMusic(char *name, int group);
extern void setMusicActive(bool_t n);
extern bool_t isMusicActive();
extern char* getCurrentMusic();
extern void delAllMusicInGroup(int group);
extern void quitMusic();

#endif

#endif
