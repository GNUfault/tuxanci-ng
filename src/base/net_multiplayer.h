
#ifndef NET_MULTIPLAYER_H

#define NET_MULTIPLAYER_H

#include "base/main.h"
#include "base/tux.h"
#include "base/item.h"
#include "base/arena.h"

#define PROTO_UDPv4	0
#define PROTO_UDPv6	1

#define NET_GAME_TYPE_NONE	0
#define NET_GAME_TYPE_SERVER	1
#define NET_GAME_TYPE_CLIENT	2

#define NET_STATUS_OK		0
#define NET_STATUS_ZOMBIE	1

#define NET_PROTOCOL_TYPE_TCP	0
#define NET_PROTOCOL_TYPE_UDP	1

#define  LIMIT_BUFFER 10240

#define NET_MASTER_SERVER_DOMAIN	"ms.tuxanci.org"
#define NET_MASTER_SERVER		"193.85.244.165"
#define NET_MASTER_PORT			2200

#ifdef PUBLIC_SERVER
extern int initNetMulitplayerPublicServer(char *ip4, int port4, char *ip6, int port6);
#endif

extern int initNetMuliplayer(int type, char *ip, int port, int proto);
extern int getNetTypeGame();
extern void eventNetMultiplayer();
extern void quitNetMultiplayer();

#endif
