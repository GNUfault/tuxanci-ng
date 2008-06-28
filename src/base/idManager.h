

#ifndef ID_MANAGER_H

#define ID_MANAGER_H

#define MAX_ID		1000
#define ID_UNKNOWN	-1

#include "base/list.h"

extern void initListID();
extern int getNewID();
extern int isRegisterID(int id);
extern void delID(int id);
extern void replaceID(int old_id, int new_id);
extern void quitListID();

#endif
