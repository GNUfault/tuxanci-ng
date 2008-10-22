
#ifndef SERVER_SEND_MSG_H

#define SERVER_SEND_MSG_H

#include "server.h"

#define PROTO_SEND_ONE			0
#define PROTO_SEND_ALL			1
#define PROTO_SEND_BUT			3
#define PROTO_SEND_ALL_SEES_TUX		4

extern void protoSendClient(int type, client_t * client, char *msg, int type2,
                            int id);

#endif
