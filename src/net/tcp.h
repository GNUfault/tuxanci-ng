
#ifndef MY_TCP_H

#    define MY_TCP_H
#    include "main.h"

#    ifndef __WIN32__
#    include <sys/socket.h>
#    include <netinet/in.h>
#    include <netinet/tcp.h>
#    else
#    include <windows.h>
#    include <wininet.h>
#    endif
#    define SUPPORT_IPv6

#    define	PROTO_TCPv4	0
#    define	PROTO_TCPv6	1

typedef struct struct_sock_tcp_t {
#   ifndef __WIN32_
	int sock;
#   else
	SOCKET sock;
#   endif
	int proto;

	struct sockaddr_in sockAddr;
#    ifdef SUPPORT_IPv6
	struct sockaddr_in6 sockAddr6;
#    endif
} sock_tcp_t;

extern sock_tcp_t *newSockTcp(void);
extern void destroySockTcp(sock_tcp_t * p);
extern sock_tcp_t *bindTcpSocket(char *addresss, int port);
extern sock_tcp_t *getTcpNewClient(sock_tcp_t * p);
extern void getSockTcpIp(sock_tcp_t * p, char *str_ip, int len);
extern int getSockTcpPort(sock_tcp_t * p);
extern sock_tcp_t *connectTcpSocket(char *ip, int port);
extern int disableNagle(sock_tcp_t * p);
extern int setTcpSockNonBlock(sock_tcp_t * p);
extern int readTcpSocket(sock_tcp_t * p, void *address, int len);
extern int writeTcpSocket(sock_tcp_t * p, void *address, int len);
extern void closeTcpSocket(sock_tcp_t * p);

#endif
