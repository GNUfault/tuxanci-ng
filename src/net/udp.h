
#ifndef MY_UDP

#    ifndef __WIN32__
#        include <netinet/in.h>
#    else
#        include <windows.h>
#        include <wininet.h>
#    endif

#    define MY_UDP

#    ifndef __WIN32__
#        define SUPPORT_IPv6
#    endif

typedef struct struct_sock_udp_t {
#    ifndef __WIN32__
	int sock;
#    else
	SOCKET sock;
#    endif
	int proto;

#    ifdef SUPPORT_IPv6
	struct sockaddr_in6 sockAddr6;
#    endif
	struct sockaddr_in sockAddr;
} sock_udp_t;

extern sock_udp_t *newSockUdp(void);
extern void destroySockUdp(sock_udp_t * p);
extern sock_udp_t *bindUdpSocket(char *address, int port);
extern sock_udp_t *connectUdpSocket(char *address, int port);
extern int setUdpSockNonBlock(sock_udp_t * p);
extern int readUdpSocket(sock_udp_t * src, sock_udp_t * dst, void *address,
						 int len);
extern int writeUdpSocket(sock_udp_t * src, sock_udp_t * dst, void *address,
						  int len);
extern void getSockUdpIp(sock_udp_t * p, char *str_ip, int len);
extern int getSockUdpPort(sock_udp_t * p);
extern void closeUdpSocket(sock_udp_t * p);
#endif
