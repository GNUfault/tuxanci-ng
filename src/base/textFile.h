
#ifndef TEXTFILE_H

#    define TEXTFILE_H

#    include "list.h"

#    ifdef __WIN32__
#        define lstat stat
#    endif

typedef struct str_textFile_t {
	char *file;
	list_t *text;
} textFile_t;

extern textFile_t *textFile_new(char *s);
extern textFile_t *textFile_load(char *s);
extern void textFile_print(textFile_t * p);
extern void textFile_save(textFile_t * p);
extern void textFile_destroy(textFile_t * p);


#endif
