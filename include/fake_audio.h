
#ifndef FAKE_AUDIO_H

#define FAKE_AUDIO_H

#ifdef NO_SOUND

#include "main.h"

#define SOUND_GROUP_BASE 0
#define SOUND_GROUP_USER 1

#define MUSIC_GROUP_BASE 0
#define MUSIC_GROUP_USER 1

extern bool_t isAudioInicialized();
extern void initAudio();
extern void quitAudio();

extern bool_t isSoundInicialized();
extern void initSound();
extern void addSound(char *file, char *name, int group);
extern void playSound(char *name, int group);
extern void setSoundActive(bool_t n);
extern bool_t isSoundActive();
extern void quitSound();

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
