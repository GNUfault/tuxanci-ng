
#ifndef DOWN_SERVER_H

#define DOWN_SERVER_H

#include "list.h"
#include "tcp.h"
#include "buffer.h"

extern int initDownServer(char *ip4, int port4);
extern int setDownServerSelect();
extern int selectDownServerSocket();
extern int quitDownServer();

#endif
