
#ifndef MY_UDP

#include <netinet/in.h>

#define MY_UDP

#define SUPPORT_IPv6

typedef struct struct_sock_udp_t
{
	int sock;
	int proto;

#ifdef SUPPORT_IPv6
	struct sockaddr_in6 sockAddr6;
#endif
	struct sockaddr_in sockAddr;
} sock_udp_t;

extern sock_udp_t* newSockUdp(int proto);
extern void destroySockUdp(sock_udp_t *p);
extern sock_udp_t* bindUdpSocket(char *address, int port, int proto);
extern sock_udp_t* connectUdpSocket(char *address, int port, int proto);
extern int readUdpSocket(sock_udp_t *src, sock_udp_t *dst, void *address, int len);
extern int writeUdpSocket(sock_udp_t *src, sock_udp_t *dst, void *address, int len);
extern void getSockUdpIp(sock_udp_t *p, char *str_ip, int len);
extern int getSockUdpPort(sock_udp_t *p);
extern void closeUdpSocket(sock_udp_t *p);

#endif
