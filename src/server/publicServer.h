#ifndef PUBLIC_SERVER_H
#define PUBLIC_SERVER_H

#include "arena.h"
#include "arenaFile.h"

#define WORLD_COUNT_ROUND_UNLIMITED	-1

extern char *public_server_get_setting(char *env, char *param, char *default_val);
extern void world_inc_round();
extern int public_server_init();
extern arenaFile_t *choice_arena_get();
extern void eventPublicServer();
extern void my_handler_quit(int n);
extern void public_server_quit();
extern int public_server_start();

#endif /* PUBLIC_SERVER_H */
