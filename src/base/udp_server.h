
#ifndef SERVER_UDP_H

#define SERVER_UDP_H

#include "server.h"
#include "udp.h"

extern client_t* newUdpClient(sock_udp_t *sock_udp);
extern void destroyUdpClient(client_t *p);
extern int initUdpServer(char *ip, int port, int proto);
extern void eventUdpServer();
extern void quitUdpServer();

#endif
