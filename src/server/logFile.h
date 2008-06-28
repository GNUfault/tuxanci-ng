
#ifndef LOGFILE_H

#define LOGFILE_H

#include "main.h"

typedef struct logFile_struct
{
	char *name;
	FILE *file;
} logFile_t;

extern logFile_t* newLogFile(char *s);
extern int writeLog(logFile_t *p, char *s);
extern void destroyLogFile(logFile_t *p);

#endif
