
#ifndef DOWN_SERVER_H

#define DOWN_SERVER_H

#include "list.h"
#include "tcp.h"
#include "buffer.h"

extern int downServer_init(char *ip4, int port4);
extern int downServer_set_select();
extern int downServer_select_socket();
extern int downServer_quit();

#endif
