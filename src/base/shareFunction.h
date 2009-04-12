
#ifndef SHARE_FUNCTION_H
#    define SHARE_FUNCTION_H

extern void shareFunction_init();
extern void shareFunction_add(char *name, void *function);
extern void *shareFunction_get(char *name);
extern void shareFunction_quit();
#endif
