
#ifndef MAIN_H

#define MAIN_H

#include "../../config.h"

#if defined PUBLIC_SERVER

#undef DESTDIR

#endif

#include "base/bool.h"
#include "base/string_length.h"
#include "base/path.h"

//rozlisenie obrazovky
#define WINDOW_SIZE_X 800
#define WINDOW_SIZE_Y 600

#include "base/list.h"

extern list_t *listHelp;

extern char* getParam(char *s);
extern char* getParamElse(char *s1, char *s2);
extern bool_t isParamFlag(char *s);
extern char *getString(int n);
extern int* newInt(int x);
extern void accessExistFile(const char *s);

#endif
