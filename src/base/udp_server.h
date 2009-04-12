
#ifndef SERVER_UDP_H

#    define SERVER_UDP_H

#    include "server.h"
#    include "udp.h"

extern client_t *serverUdp_new_client(sock_udp_t * sock_udp);
extern void serverUdp_destroy_client(client_t * p);
extern int serverUdp_init(char *ip4, int port4, char *ip6, int port6);
extern void serverUdp_set_select();
extern int serverUdp_select_sock();
extern void serverUdp_quit();

#endif
