
#ifndef ARENA_FILE_H

#define ARENA_FILE_H

#include "main.h"
#include "arena.h"

#define PUBLIC_SERVER_PATH_ARENA "./arena/"

extern bool_t isArenaFileInicialized();
extern void initArenaFile();
extern int getArenaCount();
extern char *getArenaName(int id);
extern char *getArenaNetName(int id);
extern int getArenaIdFormNetName(char *s);
extern char *getArenaImage(int id);
extern arena_t* getArena(int id);
extern void quitArenaFile();

#endif
