
#ifndef SOUND_H
#define SOUND_H
#ifndef NO_SOUND
#include <SDL_mixer.h>
#include "main.h"

#define SOUND_GROUP_BASE "base"
#define SOUND_GROUP_USER "user"

                /*
                   typedef struct sound_struct
                   {
                   char *name;
                   int group;
                   Mix_Chunk *sound;
                   } sound_t;
                 */

extern bool_t isSoundInicialized();
extern void initSound();
extern void addSound(char *file, char *name, char *group);
extern void playSound(char *name, char *group);
extern void setSoundActive(bool_t n);
extern bool_t isSoundActive();
extern void quitSound();
#endif
#endif
