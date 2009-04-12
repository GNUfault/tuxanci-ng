
#ifndef PUBLIC_SERVER_H

#    define PUBLIC_SERVER_H

#    include "arena.h"
#    include "arenaFile.h"

#    define WORLD_COUNT_ROUND_UNLIMITED	-1

extern char *publicServer_get_setting(char *env, char *param, char *default_val);
extern void word_inc_round();
extern int publicServer_init();
extern arenaFile_t *choiceArena_get();
extern void eventPublicServer();
extern void my_handler_quit(int n);
extern void publicServer_quit();
extern int publicServer_start();

#endif
