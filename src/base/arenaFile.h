
#ifndef ARENA_FILE_H
#    define ARENA_FILE_H
#    define PUBLIC_SERVER_PATH_ARENA "./arena/"

#    include "main.h"
#    include "arena.h"
#    include "textFile.h"


typedef struct {
	char *path;
	textFile_t *map;
} arenaFile_t;

extern bool_t arenaFile_is_inicialized();
extern int arenaFile_get_value(char *line, char *env, char *val, int len);
extern int arenaFile_get_count();
extern char *arenaFile_get_name(arenaFile_t * arenaFile);
extern char *arenaFile_get_net_name(arenaFile_t * arenaFile);
extern arenaFile_t *arenaFile_get_file_format_net_name(char *s);
extern char *arenaFile_get_image(arenaFile_t * arenaFile);
#    ifndef PUBLIC_SERVER
extern image_t *arenaFile_load_image(arenaFile_t * arenaFile, char *filename,
								   char *group, char *name, int alpha);
#    endif
extern arenaFile_t *arenaFile_get(int n);
extern int arenaFile_get_id(arenaFile_t * arenaFile);
extern arena_t *arenaFile_get_arena(arenaFile_t * arenaFile);
extern arenaFile_t *arenaFile_new(char *path);
extern void arenaFile_load(char *path);
extern void arenaFile_init();
extern void arenaFile_quit();

#endif
