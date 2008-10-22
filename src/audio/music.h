
#ifndef MUSIC_H
#define MUSIC_H
#ifndef NO_SOUND
#include <SDL_mixer.h>
#include "main.h"

#define MUSIC_GROUP_BASE "base"
#define MUSIC_GROUP_USER "user"

                /*
                   typedef struct music_struct
                   {
                   char *name;
                   int group;
                   Mix_Music *music;
                   } music_t;
                 */

extern bool_t isMusicInicialized();
extern void initMusic();
extern void addMusic(char *file, char *name, char *group);
extern void stopMusic();
extern void playMusic(char *name, char *group);
extern void setMusicActive(bool_t n);
extern bool_t isMusicActive();
extern char *getCurrentMusic();
extern void delAllMusicInGroup(char *group);
extern void quitMusic();
#endif
#endif
