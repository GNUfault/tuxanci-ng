
#ifndef MY_UDP

#include <netinet/in.h>

#define MY_UDP

typedef struct struct_sock_udp_t
{
	int sock;
	struct sockaddr_in sockAddr;
} sock_udp_t;

extern sock_udp_t* newSockUdp();
extern void destroySockUdp(sock_udp_t *p);
extern sock_udp_t* bindUdpSocket(int port);
extern sock_udp_t* connectUdpSocket(char *address, int port);
extern int readUdpSocket(sock_udp_t *src, sock_udp_t *dst, void *address, int len);
extern int writeUdpSocket(sock_udp_t *src, sock_udp_t *dst, void *address, int len);
extern void getSockUdpIp(sock_udp_t *p, char *str_ip);
extern int getSockUdpPort(sock_udp_t *p);
extern void closeUdpSocket(sock_udp_t *p);

#endif
