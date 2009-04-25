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

extern bool_t sound_is_inicialized();
extern void sound_init();
extern void sound_add(char *file, char *name, char *group);
extern void sound_play(char *name, char *group);
extern void sound_set_active(bool_t n);
extern bool_t sound_is_active();
extern void sound_quit();

#endif /* NO_SOUND */
#endif /* SOUND_H */
