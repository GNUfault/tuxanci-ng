
#ifndef MY_PROTO

#define MY_PROTO

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "main.h"
#include "list.h"
#include "item.h"
#include "shot.h"
#include "arenaFile.h"
#include "myTimer.h"
#include "server.h"
#include "net_multiplayer.h"

#ifndef PUBLIC_SERVER
#include "screen_world.h"
#include "screen_setting.h"
#include "screen_choiceArena.h"
#include "client.h"
#endif

#ifdef PUBLIC_SERVER
#include "publicServer.h"
#endif

#define PROTO_ERROR_CODE_UNKNOWN	0
#define PROTO_ERROR_CODE_BAD_VERSION	1
#define PROTO_ERROR_CODE_BAD_NAME	2
#define PROTO_ERROR_CODE_BAD_COMMAND	3
#define PROTO_ERROR_CODE_TIMEOUT	4
#define PROTO_ERROR_LIMIT_MAX_CLIENT	5

extern void proto_send_error_server(int type, client_t * client,
                                    int errorcode);
extern void proto_recv_error_client(char *msg);

extern void proto_send_hello_client(char *name);
extern void proto_recv_hello_server(client_t * client, char *msg);

extern void proto_send_status_server(int type, client_t * client);
extern void proto_recv_status_server(client_t * client, char *msg);

extern void proto_send_listscore_server(int type, client_t * client, int max);
extern void proto_recv_listscore_server(client_t * client, char *msg);

extern void proto_send_check_client(int id);
extern void proto_recv_check_server(client_t * client, char *msg);

extern void proto_send_init_server(int type, client_t * client,
                                   client_t * client2);
extern void proto_recv_init_client(char *msg);

extern void proto_send_event_server(int type, client_t * client, tux_t * tux,
                                    int action);
extern void proto_recv_event_client(char *msg);
extern void proto_send_event_client(int action);
extern void proto_recv_event_server(client_t * client, char *msg);

extern void proto_send_newtux_server(int type, client_t * client,
                                     tux_t * tux);
extern void proto_recv_newtux_client(char *msg);

extern void proto_send_kill_server(int type, client_t * client, tux_t * tux);
extern void proto_recv_kill_client(char *msg);

extern void proto_send_score_server(int type, client_t * client, tux_t * tux);
extern void proto_recv_score_client(char *msg);

extern void proto_send_del_server(int type, client_t * client, int id);
extern void proto_recv_del_client(char *msg);

extern void proto_send_additem_server(int type, client_t * client,
                                      item_t * p);
extern void proto_recv_additem_client(char *msg);

extern void proto_send_shot_server(int type, client_t * client, shot_t * p);
extern void proto_recv_shot_client(char *msg);

extern void proto_send_chat_client(char *s);
extern void proto_recv_chat_server(client_t * client, char *msg);
extern void proto_send_chat_server(int type, client_t * client, char *msg);
extern void proto_recv_chat_client(char *msg);

extern void proto_send_module_client(char *s);
extern void proto_recv_module_server(client_t * client, char *msg);
extern void proto_send_module_server(int type, client_t * client, char *msg);
extern void proto_recv_module_client(char *msg);

extern void proto_send_ping_client();
extern void proto_recv_ping_server(client_t * client, char *msg);
extern void proto_send_ping_server(int type, client_t * client);
extern void proto_recv_ping_client(char *msg);

extern void proto_recv_echo_server(client_t * client, char *msg);
extern void proto_send_echo_server(int type, client_t * client, char *s);

extern void proto_send_end_server(int type, client_t * client);
extern void proto_recv_end_client(char *msg);
extern void proto_send_end_client();
extern void proto_recv_end_server(client_t * client, char *msg);

#endif
