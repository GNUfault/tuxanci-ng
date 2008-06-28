
#ifndef CHECK_FRONT_H

#define CHECK_FRONT_H

#include "base/list.h"
#include "base/server.h"

#define CHECK_FRONT_SEND_TIME_INTERVAL	100
#define CHECK_FRONT_MAX_COUNT_SEND	30

#define CHECK_FRONT_ID_NONE	-1
#define CHECK_FORNT_TYPE_SIMPLE		1
#define CHECK_FORNT_TYPE_CHECK		2

extern list_t* newCheckFront();
extern void addMsgInCheckFront(list_t *list, char *msg, int type, int id);
extern void eventMsgInCheckFront(client_t *client);
extern void delMsgInCheckFront(list_t *listCheckFront, int id);
extern void destroyCheckFront(list_t *p);

#endif
