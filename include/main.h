
#ifndef MAIN_H

#define MAIN_H

#include "bool.h"
#include "string_length.h"
#include "path.h"

extern char* getParam(char *s);
extern char* getParamElse(char *s1, char *s2);
extern bool_t isParamFlag(char *s);
extern char *getString(int n);
extern void quit();

#endif
