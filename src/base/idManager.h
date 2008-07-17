

#ifndef ID_MANAGER_H

#define ID_MANAGER_H

#define MAX_ID		1000000
#define ID_UNKNOWN	-1

#ifdef __WIN32__
#define random rand
#endif

#include "list.h"

extern void initListID();
extern int isRegisterID(int id);
extern int getNewIDcount(int count);
extern int getNewID();
extern void incID(int id);
extern void delID(int id);
extern void replaceID(int old_id, int new_id);
extern void quitListID();

#endif
