
#ifndef ARENA_FILE_H
#define ARENA_FILE_H
#define PUBLIC_SERVER_PATH_ARENA "./arena/"

#include "main.h"
#include "arena.h"
#include "textFile.h"


typedef struct
{
    char *path;
    textFile_t *map;
} arenaFile_t;

extern bool_t isAreaFileInicialized();
extern int getArenaValue(char *line, char *env, char *val, int len);
extern int getArenaCount();
extern char *getArenaName(arenaFile_t * arenaFile);
extern char *getArenaNetName(arenaFile_t * arenaFile);
extern arenaFile_t *getArenaFileFormNetName(char *s);
extern char *getArenaImage(arenaFile_t * arenaFile);
#ifndef PUBLIC_SERVER
extern image_t *loadImageFromArena(arenaFile_t * arenaFile, char *filename,
                                   char *group, char *name, int alpha);
#endif
extern arenaFile_t *getArenaFile(int n);
extern int getArenaFileID(arenaFile_t * arenaFile);
extern arena_t *getArena(arenaFile_t * arenaFile);
extern arenaFile_t *newArenaFile(char *path);
extern void loadArenaFile(char *path);
extern void initArenaFile();
extern void quitArenaFile();

#endif
