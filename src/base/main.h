
#ifndef MAIN_H

#define MAIN_H

#include "../../config.h"

#include "bool.h"
#include "string_length.h"
#include "path.h"

// screen resolution
#define WINDOW_SIZE_X 800
#define WINDOW_SIZE_Y 600

#include "list.h"

extern list_t *listHelp;

extern char* getParam(char *s);
extern char* getParamElse(char *s1, char *s2);
extern bool_t isParamFlag(char *s);
extern char *getString(int n);
extern int* newInt(int x);
extern void accessExistFile(const char *s);
extern int tryExistFile (const char *s);

#endif
