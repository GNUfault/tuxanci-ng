
#ifndef NET_MULTIPLAYER_H

#define NET_MULTIPLAYER_H

#include "main.h"
#include "tux.h"
#include "item.h"
#include "buffer.h"
#include "arena.h"
#include "tcp.h"
#include "udp.h"

#define NET_GAME_TYPE_NONE	0
#define NET_GAME_TYPE_SERVER	1
#define NET_GAME_TYPE_CLIENT	2

#define NET_STATUS_OK		0
#define NET_STATUS_ZOMBIE	1

#define NET_PROTOCOL_TYPE_TCP	0
#define NET_PROTOCOL_TYPE_UDP	1

#define  LIMIT_BUFFER 10240

#ifdef PUBLIC_SERVER
extern int initNetMulitplayerPublicServer(char *ip4, int port4, char *ip6, int port6);
#endif

extern int initNetMuliplayer(int type, char *ip, int port, int proto);
extern int getNetTypeGame();
extern void eventNetMultiplayer();
extern void quitNetMultiplayer();

#endif
