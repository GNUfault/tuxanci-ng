#ifndef MY_UDP_H
#define MY_UDP_H

#ifndef __WIN32__
#include <netinet/in.h>
#else
#include <windows.h>
#include <wininet.h>
#endif

typedef struct struct_sock_udp_t {
#ifndef __WIN32__
	int sock;
#else
	SOCKET sock;
#endif
	int proto;

#ifdef SUPPORT_IPv6
	struct sockaddr_in6 sockAddr6;
#endif
	struct sockaddr_in sockAddr;
} sock_udp_t;

extern sock_udp_t *sock_udp_new(void);
extern void sock_udp_destroy(sock_udp_t *p);
extern sock_udp_t *sock_udp_bind(char *address, int port);
extern sock_udp_t *sock_udp_connect(char *address, int port);
extern int sock_udp_set_non_block(sock_udp_t *p);
extern int sock_udp_read(sock_udp_t *src, sock_udp_t *dst, void *address, int len);
extern int sock_udp_write(sock_udp_t *src, sock_udp_t *dst, void *address, int len);
extern void sock_udp_get_ip(sock_udp_t *p, char *str_ip, int len);
extern int sock_udp_get_port(sock_udp_t *p);
extern void sock_udp_close(sock_udp_t *p);

#endif /* MY_UDP_H */
