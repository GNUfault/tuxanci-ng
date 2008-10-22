
#ifndef MY_TCP_H

#define MY_TCP_H
#include "main.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

#define SUPPORT_IPv6

#define	PROTO_TCPv4	0
#define	PROTO_TCPv6	1

typedef struct struct_sock_tcp_t
{
    int sock;
    int proto;

    struct sockaddr_in sockAddr;
#ifdef SUPPORT_IPv6
    struct sockaddr_in6 sockAddr6;
#endif
} sock_tcp_t;

extern sock_tcp_t *newSockTcp(int proto);
extern void destroySockTcp(sock_tcp_t * p);
extern sock_tcp_t *bindTcpSocket(char *addresss, int port, int proto);
extern sock_tcp_t *getTcpNewClient(sock_tcp_t * p);
extern void getSockTcpIp(sock_tcp_t * p, char *str_ip, int len);
extern int getSockTcpPort(sock_tcp_t * p);
extern sock_tcp_t *connectTcpSocket(char *ip, int port, int proto);
extern int disableNagle(sock_tcp_t * p);
extern int setTcpSockNonBlock(sock_tcp_t * p);
extern int readTcpSocket(sock_tcp_t * p, void *address, int len);
extern int writeTcpSocket(sock_tcp_t * p, void *address, int len);
extern void closeTcpSocket(sock_tcp_t * p);

#endif
