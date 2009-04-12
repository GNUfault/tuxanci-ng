
#ifndef SERVER_TCP_H

#    define SERVER_TCP_H

#    include "server.h"
#    include "tcp.h"

#    define SERVER_TCP_BUFFER_LIMIT	4096

extern client_t *serverTcp_new_client(sock_tcp_t * sock_tcp);
extern void serverTcp_destroy_client(client_t * p);
extern int serverTcp_init(char *ip4, int port4, char *ip6, int port6);
extern void serverTcp_set_select();
extern int serverTcp_select_sock();
extern void serverTcp_send_client_buffer(client_t * p);
extern void serverTcp_quit();

#endif
