
#ifndef MY_SERVER

#define MY_SERVER

#include <time.h>

#include "list.h"
#include "tux.h"
#include "buffer.h"
#include "myTimer.h"

#ifndef PUBLIC_SERVER
#include "interface.h"
#endif


#ifdef SUPPORT_NET_SDL_UDP

#include "sdl_udp.h"

#endif

#ifdef SUPPORT_NET_UNIX_UDP

#include "udp.h"

#endif

#define SERVER_TIMEOUT		5000
#define SERVER_TIME_SYNC	1000
#define SERVER_TIME_PING	1000
#define SERVER_MAX_CLIENTS	100


#define SERVER_INDEX_ROOT_TUX	0

typedef struct client_struct
{
#ifdef SUPPORT_NET_UNIX_UDP
	sock_udp_t *socket_udp;
#endif

#ifdef SUPPORT_NET_SDL_UDP
	sock_sdl_udp_t *socket_sdl_udp;
#endif
	int status;
	tux_t *tux;
	my_time_t lastPing;
	list_t *listCheck;
	list_t *buffer;
} client_t;

#ifdef PUBLIC_SERVER
extern int initUdpPublicServer(char *ip4, int port4, char *ip6, int port6);
#endif

#ifdef SUPPORT_NET_UNIX_UDP
extern int initUdpServer(char *ip, int port, int proto);
extern int initUdpServerMultiProto(char *ip4, char *ip6, int port4, int port6);
extern client_t* newUdpClient(sock_udp_t *sock_udp);
extern int selectServerUdpSocket();
extern void quitUdpServer();
#endif

#ifdef SUPPORT_NET_SDL_UDP
extern int initSdlUdpServer(int port);
extern client_t* newSdlUdpClient(sock_sdl_udp_t *sock_udp);
extern void selectServerSdlUdpSocket();
extern void quitSdlUdpServer();
#endif

extern void destroyClient(client_t *p);

extern time_t getUpdateServer();
extern list_t* getListServerClient();
extern int getServerMaxClients();
extern void setServerMaxClients(int n);

extern void sendClient(client_t *p, char *msg);
extern void protoSendClient(int type, client_t *client, char *msg, int type2, int id);

extern tux_t *getServerTux();
extern client_t *getClientFromTux(tux_t *tux);
extern void eventClientListBuffer();
extern void sendInfoCreateClient(client_t *client);
extern void eventServer();

#endif 
 
