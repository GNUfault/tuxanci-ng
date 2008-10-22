
#include <dirent.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>

#include "main.h"
#include "list.h"

typedef struct director_struct
{
	char *path;
	list_t *list;
} director_t;

director_t* loadDirector(char *s)
{
	director_t *new;
	DIR *dir;
	struct dirent *item;
	char path[STR_PATH_SIZE];
	assert( s != 0 );
	new = malloc( sizeof(director_t) );
	memset(new, 0, sizeof(director_t) );
	new->list = newList();
#ifndef __WIN32__
	if( s[0] == '/' )
#else
	if( s[1] == ':' )
#endif
		strcpy(path, s);
	else {
		// this is really correct aproach
		getcwd(path, STR_PATH_SIZE);
		strcat(path, "/");
		strcat(path, s);
	}
	new->path = strdup(path);
	dir = opendir(new->path);
	if( dir == NULL ) {
		free(new->path);
		free(new);
		return NULL;
	}

	while((item = readdir(dir)) != NULL)
		addList(new->list, strdup(item->d_name) );
	closedir(dir);
	return new;
}

void destroyDirector(director_t *p)
{
	assert( p != NULL );
	destroyListItem(p->list, free);
	free(p->path);
	free(p);
}
