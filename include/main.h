
#ifndef MAIN_H

#define MAIN_H

#include "../config.h"

#if defined SUPPORT_NET_UNIX_UDP && defined SUPPORT_NET_SDL_UDP

#undef SUPPORT_NET_SDL_UDP

#endif

#include "bool.h"
#include "string_length.h"
#include "path.h"

//rozlisenie obrazovky
#define WINDOW_SIZE_X 800
#define WINDOW_SIZE_Y 600

extern char* getParam(char *s);
extern char* getParamElse(char *s1, char *s2);
extern bool_t isParamFlag(char *s);
extern char *getString(int n);
extern void quit();

#endif
