
#ifndef MY_SERVER

#define MY_SERVER

#include "list.h"
#include "tux.h"
#include "buffer.h"
#include "myTimer.h"

#ifndef BUBLIC_SERVER
#include "interface.h"
#endif

#ifdef SUPPORT_NET_UNIX_TCP
#include "tcp.h"
#endif

#ifdef SUPPORT_NET_UNIX_UDP

#include "udp.h"

#define SERVER_TIMEOUT		5000
#define SERVER_TIME_SYNC	1000
#define SERVER_MAX_CLIENTS	100

#endif

#ifdef SUPPORT_NET_SDL_UDP

#include "sdl_udp.h"

#define SERVER_TIMEOUT		5000
#define SERVER_TIME_SYNC	1000
#define SERVER_MAX_CLIENTS	100

#endif


#define SERVER_INDEX_ROOT_TUX	0

typedef struct client_struct
{
#ifdef SUPPORT_NET_UNIX_TCP
	sock_tcp_t *socket_tcp;
#endif

#ifdef SUPPORT_NET_UNIX_UDP
	sock_udp_t *socket_udp;
	my_time_t lastPing;
#endif

#ifdef SUPPORT_NET_SDL_UDP
	sock_sdl_udp_t *socket_sdl_udp;
	my_time_t lastPing;
#endif
	int status;
	tux_t *tux;
	buffer_t *buffer;
} client_t;

#ifdef SUPPORT_NET_UNIX_TCP
extern int initTcpServer(int port);
extern client_t* newTcpClient(sock_tcp_t *sock_tcp);
extern void selectServerTcpSocket();
extern void quitTcpServer();
#endif

#ifdef SUPPORT_NET_UNIX_UDP
extern int initUdpServer(int port);
extern client_t* newUdpClient(sock_udp_t *sock_udp);
extern void selectServerUdpSocket();
extern void quitUdpServer();
#endif

#ifdef SUPPORT_NET_SDL_UDP
extern int initSdlUdpServer(int port);
extern client_t* newSdlUdpClient(sock_sdl_udp_t *sock_udp);
extern void selectServerSdlUdpSocket();
extern void quitSdlUdpServer();
#endif

extern void destroyClient(client_t *p);

extern list_t* getListServerClient();
extern int getServerMaxClients();
extern void setServerMaxClients(int n);

extern void sendClient(client_t *p, char *msg);
extern void sendAllClientBut(char *msg, client_t *p);
extern void sendAllClient(char *msg);
extern void eventClientListBuffer();
extern void sendInfoCreateClient(client_t *client);
extern void eventPeriodicSyncClient();

#endif 
 
