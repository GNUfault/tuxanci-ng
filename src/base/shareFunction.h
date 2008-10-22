
#ifndef SHARE_FUNCTION_H
#define SHARE_FUNCTION_H

extern void initShareFunction();
extern void addToShareFce(char *name, void *function);
extern void *getShareFce(char *name);
extern void quitShareFunction();
#endif
