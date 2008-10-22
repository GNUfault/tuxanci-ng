
#ifndef MAIN_H
#define MAIN_H
#define WINDOW_SIZE_X 800
#define WINDOW_SIZE_Y 600
#ifdef NLS
#define _(text) gettext(text)
#else
#define _(text) (text)
#endif

#ifdef NLS
#include <libintl.h>
#include <locale.h>
#endif
#include "bool.h"
#include "string_length.h"
#include "path.h"

extern char *getParam(char *s);
extern char *getParamElse(char *s1, char *s2);
extern bool_t isParamFlag(char *s);
extern char *getString(int n);
extern int *newInt(int x);
extern int isFillPath(const char *path);
extern void accessExistFile(const char *s);
extern int tryExistFile(const char *s);
#endif
