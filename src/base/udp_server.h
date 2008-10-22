
#ifndef SERVER_UDP_H

#define SERVER_UDP_H

#include "server.h"
#include "udp.h"

extern client_t *newUdpClient(sock_udp_t * sock_udp);
extern void destroyUdpClient(client_t * p);
extern int initUdpServer(char *ip4, int port4, char *ip6, int port6);
extern void setServerUdpSelect();
extern int selectServerUdpSocket();
extern void quitUdpServer();

#endif
