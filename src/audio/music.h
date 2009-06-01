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

extern bool_t music_is_inicialized();
extern void music_init();
extern void music_add(char *file, char *name, char *group);
extern void music_stop();
extern void music_play(char *name, char *group);
extern void music_set_active(bool_t n);
extern bool_t music_is_active();
extern char *music_get_current();
extern void music_del_all_in_group(char *group);
extern void music_quit();

#endif /* NO_SOUND */
#endif /* MUSIC_H */
