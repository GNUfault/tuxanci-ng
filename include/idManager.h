

#ifndef ID_MANAGER_H

#define ID_MANAGER_H

#define MAX_ID	1000

#include "list.h"

extern list_t * newListID();
extern int getNewID(list_t *p);
extern int isRegisterID(list_t *p, int id);
extern void delID(list_t *p, int id);
extern void replaceID(list_t *p, int old_id, int new_id);
extern void destroyListID(list_t *p);

#endif
