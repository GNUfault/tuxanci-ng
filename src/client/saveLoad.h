
#ifndef SAVELOAD_H

#define SAVELOAD_H

#include "textFile.h"
#include "arena.h"

extern void saveArena(char *filename, arena_t * arena);
extern void loadArena(char *filename);

#endif
