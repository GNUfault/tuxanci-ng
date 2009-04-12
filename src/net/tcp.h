
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

extern sock_tcp_t *sock_tcp_new(void);
extern void sock_tcp_destroy(sock_tcp_t * p);
extern sock_tcp_t *sock_tcp_bind(char *addresss, int port);
extern sock_tcp_t *sock_tcp_accept(sock_tcp_t * p);
extern void sock_tcp_get_ip(sock_tcp_t * p, char *str_ip, int len);
extern int sock_tcp_get_port(sock_tcp_t * p);
extern sock_tcp_t *sock_tcp_connect(char *ip, int port);
extern int sock_tcp_diable_nagle(sock_tcp_t * p);
extern int sock_tcp_set_non_block(sock_tcp_t * p);
extern int sock_tcp_read(sock_tcp_t * p, void *address, int len);
extern int sock_tcp_write(sock_tcp_t * p, void *address, int len);
extern void sock_tcp_close(sock_tcp_t * p);

#endif
