
#ifndef MY_SERVER

#    define MY_SERVER

#    include <time.h>

#    include "list.h"
#    include "tux.h"
#    include "myTimer.h"
#    include "protect.h"
#    include "buffer.h"

#    ifndef PUBLIC_SERVER
#        include "interface.h"
#    endif

#    include "udp.h"

#    define SERVER_TIMEOUT		1000
#    define SERVER_TIME_SYNC	1000
#    define SERVER_TIME_PING	1000
#    define SERVER_MAX_CLIENTS	100

#    define SERVER_INDEX_ROOT_TUX	0

#    define CLIENT_TYPE_UDP		1
#    define CLIENT_TYPE_TCP		2

typedef struct client_struct {
	int type;

	sock_udp_t *socket_udp;

	buffer_t *recvBuffer;
	buffer_t *sendBuffer;

	int status;

	tux_t *tux;

	protect_t *protect;

	list_t *listSendMsg;
	list_t *listRecvMsg;

	list_t *listSeesShot;
} client_t;

extern int server_init(char *ip4, int port4, char *ip6, int port6);
extern time_t server_get_update();
extern client_t *server_new_any_client();
extern void server_destroy_any_client(client_t * p);
extern list_t *server_get_list_clients();
extern int server_get_max_clients();
extern void server_set_time();
extern void server_set_max_clients(int n);
extern void server_send_client(client_t * p, char *msg);
extern void server_event();
extern void server_quit();

#endif
