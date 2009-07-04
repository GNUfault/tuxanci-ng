#ifndef MAIN_H
#define MAIN_H

#ifdef NLS
#include <libintl.h>
#include <locale.h>
#endif /* NLS */

#include "bool.h"
#include "string_length.h"
#include "path.h"

#define WINDOW_SIZE_X		800
#define WINDOW_SIZE_Y		600

#ifdef NLS
#define _(text)			gettext(text)
#else /* NLS */
#define _(text)			(text)
#endif /* NLS */

#include "msg.h"

#define UNUSED(var)		if (0 && var) {}

extern char *getParam(char *s);
extern char *getParamElse(char *s1, char *s2);
extern bool_t isParamFlag(char *s);
extern char *getString(int n);
extern int *newInt(int x);
extern int isFillPath(const char *path);
extern void accessExistFile(const char *s);
extern int tryExistFile(const char *s);

#endif /* MAIN_H */
