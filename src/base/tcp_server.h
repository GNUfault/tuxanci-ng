
#ifndef SERVER_TCP_H

#define SERVER_TCP_H

#include "server.h"
#include "tcp.h"

#define SERVER_TCP_BUFFER_LIMIT	4096

extern client_t *newTcpClient(sock_tcp_t * sock_tcp);
extern void destroyTcpClient(client_t * p);
extern int initTcpServer(char *ip4, int port4, char *ip6, int port6);
extern void setServerTcpSelect();
extern int selectServerTcpSocket();
extern void sendTcpClientBuffer(client_t * p);
extern void quitTcpServer();

#endif
