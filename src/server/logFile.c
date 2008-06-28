
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <assert.h>

typedef struct logFile_struct
{
	char *name;
	FILE *file;
} logFile_t;

logFile_t* newLogFile(char *s)
{
	logFile_t *new;

	assert( s != NULL );

	new  = malloc( sizeof(logFile_t) );
	memset(new, 0, sizeof(logFile_t) );
	
	new->name = strdup(s);
	new->file = fopen(new->name, "a+");

	assert( new->file != NULL );

	return new;
}

int writeLog(logFile_t *p, char *s)
{
	assert( p != NULL );
	assert( s != NULL );

	return fprintf(p->file, s);
}

void destroyLogFile(logFile_t *p)
{
	assert( p != NULL );

	free(p->name);
	fclose(p->file);
	free(p);
}
