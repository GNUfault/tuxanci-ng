
#ifndef MY_TCP_H

#define MY_TCP_H

#include <netinet/in.h>

typedef struct struct_sock_tcp_t
{
	int sock;
	struct sockaddr_in sockAddr;
} sock_tcp_t;

extern sock_tcp_t* newSockTcp();
extern void destroySockTcp(sock_tcp_t *p);
extern sock_tcp_t* bindTcpSocket(int port);
extern sock_tcp_t* getTcpNewClient(sock_tcp_t *p);
extern void getSockTcpIp(sock_tcp_t *p, char *str_ip);
extern int getSockTcpPort(sock_tcp_t *p);
extern sock_tcp_t* connectTcpSocket(char *ip, int port);
extern int readTcpSocket(sock_tcp_t *p, void *address, int len);
extern int writeTcpSocket(sock_tcp_t *p, void *address, int len);
extern void closeTcpSocket(sock_tcp_t *p);

#endif
