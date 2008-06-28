
#ifndef ARENA_FILE_H

#define ARENA_FILE_H

#include "base/main.h"
#include "base/arena.h"

#define PUBLIC_SERVER_PATH_ARENA "./arena/"

extern int getArenaValue(char *line, char *env, char *val, int len);
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
