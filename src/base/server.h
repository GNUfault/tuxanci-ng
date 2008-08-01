
#ifndef MY_SERVER

#define MY_SERVER

#include <time.h>

#include "list.h"
#include "tux.h"
#include "myTimer.h"
#include "protect.h"
#include "udp.h"
#include "tcp.h"
#include "buffer.h"

#ifndef PUBLIC_SERVER
#include "interface.h"
#endif

#define SERVER_TIMEOUT		1000
#define SERVER_TIME_SYNC	1000
#define SERVER_TIME_PING	1000
#define SERVER_MAX_CLIENTS	100


#define SERVER_INDEX_ROOT_TUX	0

#define CLIENT_TYPE_UDP		1
#define CLIENT_TYPE_TCP		2

typedef struct client_struct
{
	int type;
	sock_udp_t *socket_udp;
	sock_tcp_t *socket_tcp;
	buffer_t *buffer;

	int status;

	tux_t *tux;

	protect_t *protect;

	list_t *listSendMsg;
	list_t *listRecvMsg;

	list_t *listSeesShot;
} client_t;

extern int initServer(char *ip, int port, int proto);
extern time_t getUpdateServer();
extern client_t* newAnyClient();
extern void destroyAnyClient(client_t *p);
extern list_t* getListServerClient();
extern int getServerMaxClients();
extern void setServerTimer();
extern void setServerMaxClients(int n);
extern void sendInfoCreateClient(client_t *client);
extern void sendClient(client_t *p, char *msg);
extern void protoSendClient(int type, client_t *client, char *msg, int type2, int id);
extern void eventServer();
extern void quitServer();

#endif
