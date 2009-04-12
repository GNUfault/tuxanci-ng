
#ifndef CHECK_FRONT_H

#    define CHECK_FRONT_H
#    define CHECK_FRONT_SEND_TIME_INTERVAL	100
#    define CHECK_FRONT_MAX_COUNT_SEND		30

#    define CHECK_FRONT_ID_NONE				-1
#    define CHECK_FRONT_TYPE_SIMPLE			1
#    define CHECK_FRONT_TYPE_CHECK			2

#    include "list.h"
#    include "server.h"

extern list_t *checkFront_new();
extern void checkFront_add_msg(list_t * list, char *msg, int type, int id);
extern void checkFront_event(client_t * client);
extern void checkFront_del_msg(list_t * listCheckFront, int id);
extern void checkFront_destroy(list_t * p);

#endif
