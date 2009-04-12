
#ifndef SAVELOAD_H

#    define SAVELOAD_H

#    include "textFile.h"
#    include "arena.h"

extern void save_arena(char *filename, arena_t * arena);
extern void load_arena(char *filename);

#endif
