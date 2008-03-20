
#ifndef MY_PROTO

#define MY_PROTO

#include "main.h"
#include "tux.h"
#include "server.h"
#include "item.h"
#include "shot.h"

#define PROTO_SEND_ONE	0
#define PROTO_SEND_ALL	1
#define PROTO_SEND_BUT	2

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "main.h"
#include "list.h"
#include "tux.h"
#include "item.h"
#include "shot.h"
#include "arenaFile.h"
#include "myTimer.h"
#include "server.h"
#include "proto.h"
#include "net_multiplayer.h"

#ifndef BUBLIC_SERVER
#include "network.h"
#include "screen_world.h"
#include "screen_setting.h"
#include "screen_choiceArena.h"
#include "client.h"
#endif

#ifdef BUBLIC_SERVER
#include "publicServer.h"
#endif

extern void proto_send_hello_client(char *name);
extern void proto_recv_hello_server(client_t *client, char *msg);
extern void proto_send_status_server(int type, client_t *client);
extern void proto_recv_status_server(client_t *client, char *msg);
extern void proto_send_init_server(int type, client_t *client, client_t *client2);
extern void proto_recv_init_client(char *msg);
extern void proto_send_event_server(int type, client_t *client, tux_t *tux, int action);
extern void proto_recv_event_client(char *msg);
extern void proto_send_event_client(int action);
extern void proto_recv_event_server(client_t *client, char *msg);
extern void proto_send_newtux_server(int type, client_t *client, tux_t *tux);
extern void proto_recv_newtux_client(char *msg);
extern void proto_send_kill_server(int type, client_t *client, tux_t *tux);
extern void proto_recv_kill_client(char *msg);
extern void proto_send_score_server(int type, client_t *client, tux_t *tux);
extern void proto_recv_score_client(char *msg);
extern void proto_send_deltux_server(int type, client_t *client, client_t *client2);
extern void proto_recv_deltux_client(char *msg);
extern void proto_send_additem_server(int type, client_t *client, item_t *p);
extern void proto_recv_additem_client(char *msg);
extern void proto_send_shot_server(int type, client_t *client, shot_t *p);
extern void proto_recv_shot_client(char *msg);
extern void proto_send_context_client(tux_t *tux);
extern void proto_recv_context_server(client_t *client, char *msg);
extern void proto_send_ping_client();
extern void proto_recv_ping_server(client_t *client, char *msg);
extern void proto_send_end_client();
extern void proto_recv_end_server(client_t *client, char *msg);
extern void proto_send_ping_server(int type, client_t *client);
extern void proto_recv_ping_client(char *msg);
extern void proto_send_end_server(int type, client_t *client);
extern void proto_recv_end_client(char *msg);

#endif

