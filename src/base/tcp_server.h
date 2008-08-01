
#ifndef SERVER_TCP_H

#define SERVER_TCP_H

#include "server.h"
#include "tcp.h"

extern client_t* newTcpClient(sock_tcp_t *sock_tcp);
extern void destroyTcpClient(client_t *p);
extern int initTcpServer(char *ip, int port, int proto);
extern void eventTcpServer();
extern void quitTcpServer();

#endif
