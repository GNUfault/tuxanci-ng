
#ifndef TEXTFILE_H

#define TEXTFILE_H

#include "list.h"

#ifdef __WIN32__
#define lstat stat
#endif

typedef struct str_textFile_t
{
    char *file;
    list_t *text;
} textFile_t;

extern textFile_t *newTextFile(char *s);
extern textFile_t *loadTextFile(char *s);
extern void printTextFile(textFile_t * p);
extern void saveTextFile(textFile_t * p);
extern void destroyTextFile(textFile_t * p);


#endif
