
#ifndef NET_MULTIPLAYER_H

#define NET_MULTIPLAYER_H

#include "tux.h"
#include "item.h"
#include "buffer.h"
#include "arena.h"

#define NET_GAME_TYPE_NONE	0
#define NET_GAME_TYPE_SERVER	1
#define NET_GAME_TYPE_CLIENT	2

#define NET_STATUS_OK		0
#define NET_STATUS_ZOMBIE	1

#define  LIMIT_BUFFER 10240

typedef struct client_struct
{
	int socket;
	int status;

	tux_t *tux;
	buffer_t *buffer;
} client_t;

extern int initServer(int port);
extern int initClient(int port, char *ip);
extern void netSetArena(arena_t *p);
extern int getNetTypeGame();

extern void proto_send_init_server(client_t *client);
extern void proto_recv_init_client(char *msg);
extern void proto_send_event_server(tux_t *tux, int action, client_t *client);
extern void proto_recv_event_client(char *msg);
extern void proto_send_event_client(int action);
extern void proto_recv_event_server(client_t *client, char *msg);
extern void proto_send_newtux_server(client_t *client, tux_t *tux);
extern void proto_recv_newtux_client(char *msg);
extern void proto_send_deltux_server(client_t *client);
extern void proto_recv_deltux_client(char *msg);
extern void proto_send_additem_server(item_t *p);
extern void proto_recv_additem_client(char *msg);
extern void proto_send_context_client(tux_t *tux);
extern void proto_recv_context_server(client_t *client, char *msg);

extern void eventNetMultiplayer();
extern void quitNetMultiplayer();

#endif
