#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <fcntl.h>
#include <sys/types.h>
#include <sys/time.h>

#ifndef __WIN32__
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/select.h>
#else /* __WIN32__ */
#include <io.h>
#include <winsock2.h>
#endif /* __WIN32__ */

#include "main.h"
#include "list.h"
#include "tux.h"
#include "proto.h"

#include "client.h"

#include "analyze.h"
#include "world.h"
#include "buffer.h"

#include "udp.h"

static sock_udp_t *sock_server_udp;

static list_t *listRecvMsg;

static buffer_t *clientRecvBuffer;
static buffer_t *clientSendBuffer;

static my_time_t lastPing;
static my_time_t lastPingServerAlive;

#ifdef SUPPORT_TRAFFIC
static my_time_t lastTraffic;
static int traffic_down;
static int traffic_up;
#endif /* SUPPORT_TRAFFIC */

typedef struct proto_cmd_client_struct {
	char *name;
	int len;
	void (*fce_proto) (char *msg);
} proto_cmd_client_t;

static proto_cmd_client_t proto_cmd_list[] = {
	{.name = "error",.len = 5,.fce_proto = proto_recv_error_client},
	{.name = "init",.len = 4,.fce_proto = proto_recv_init_client},
	{.name = "event",.len = 5,.fce_proto = proto_recv_event_client},
	{.name = "newtux",.len = 6,.fce_proto = proto_recv_newtux_client},
	{.name = "del",.len = 3,.fce_proto = proto_recv_del_client},
	{.name = "additem",.len = 7,.fce_proto = proto_recv_additem_client},
	{.name = "shot",.len = 4,.fce_proto = proto_recv_shot_client},
	{.name = "kill",.len = 4,.fce_proto = proto_recv_kill_client},
	{.name = "module",.len = 6,.fce_proto = proto_recv_module_client},
	{.name = "chat",.len = 4,.fce_proto = proto_recv_chat_client},
	{.name = "ping",.len = 4,.fce_proto = proto_recv_ping_client},
	{.name = "end",.len = 3,.fce_proto = proto_recv_end_client},
	{.name = "",.len = 0,.fce_proto = NULL}
};

static proto_cmd_client_t *findCmdProto(char *msg)
{
	int len;
	int i;

	len = strlen(msg);

	for (i = 0; proto_cmd_list[i].len != 0; i++) {
		proto_cmd_client_t *thisCmd;

		thisCmd = &proto_cmd_list[i];

		if (len >= thisCmd->len &&
		    strncmp(msg, thisCmd->name, thisCmd->len) == 0) {
			return thisCmd;
		}
	}

	return NULL;
}

static int initUdpClient(char *ip, int port)
{
	sock_server_udp = sock_udp_connect(ip, port);

	if (sock_server_udp == NULL) {
		return -1;
	}

	DEBUG_MSG(_("[Debug] Connected to game server [%s]:[%d]\n"), ip, port);

	return 0;
}

int client_init(char *ip, int port)
{
	char name[STR_NAME_SIZE];
	int ret;

	listRecvMsg = list_new();
	lastPing = timer_get_current_time();
	lastPingServerAlive = timer_get_current_time();

#ifdef SUPPORT_TRAFFIC
	lastTraffic = timer_get_current_time();
	traffic_down = 0;
	traffic_up = 0;
#endif /* SUPPORT_TRAFFIC */

	clientRecvBuffer = buffer_new(CLIENT_BUFFER_LIMIT);
	clientSendBuffer = buffer_new(CLIENT_BUFFER_LIMIT);

	ret = initUdpClient(ip, port);

	if (ret < 0) {
		return -1;
	}

	public_server_get_setting_name_right(name);
	proto_send_hello_client(name);

	return 0;
}

static void errorWithServer()
{
	fprintf(stderr, _("[Error] Game server is not responding\n"));
	analyze_set_msg(_("[Error] Unable to connect to the game server"));
	world_do_end();
}

void client_send(char *msg)
{
	int ret;

	assert(msg != NULL);

	ret = -1;

	if (isParamFlag("--send")) {
		printf(_("[Debug] Sending data: %s"), msg);
	}

#ifdef SUPPORT_TRAFFIC
	traffic_up += strlen(msg);
#endif /* SUPPORT_TRAFFIC */

	if (sock_server_udp != NULL) {
		ret = sock_udp_write(sock_server_udp, sock_server_udp, msg, strlen(msg));
	}

	if (ret < 0) {
		errorWithServer();
		return;
	}
}

static int server_eventSelect()
{
	char buffer[STR_PROTO_SIZE];
	int ret;

	memset(buffer, 0, STR_PROTO_SIZE);
	ret = -1;

	if (sock_server_udp != NULL) {
		ret = sock_udp_read(sock_server_udp, sock_server_udp, buffer, STR_PROTO_SIZE - 1);
	}

	if (ret < 0) {
		errorWithServer();
		return ret;
	}

#ifdef SUPPORT_TRAFFIC
	traffic_down += ret;
#endif /* SUPPORT_TRAFFIC */

	list_add(listRecvMsg, strdup(buffer));

	return ret;
}

static void client_eventWorkRecvList()
{
	proto_cmd_client_t *protoCmd;
	char *line;
	int i;

	/* processing of an event received from a game server */

	assert(listRecvMsg != NULL);

	for (i = 0; i < listRecvMsg->count; i++) {
		line = (char *) listRecvMsg->list[i];
		protoCmd = findCmdProto(line);

		if (isParamFlag("--recv")) {
			printf(_("[Debug] Recieved data: %s"), line);
		}

		if (protoCmd != NULL) {
			protoCmd->fce_proto(line);
			lastPingServerAlive = timer_get_current_time();
		}
	}

	list_destroy_item(listRecvMsg, free);
	listRecvMsg = list_new();
}

static void eventPingServer()
{
	my_time_t currentTime;

	currentTime = timer_get_current_time();

	if (currentTime - lastPing > CLIENT_TIMEOUT) {
		proto_send_ping_client();
		lastPing = timer_get_current_time();
	}
}

static bool_t isServerAlive()
{
	my_time_t currentTime;

	currentTime = timer_get_current_time();

	if (currentTime - lastPingServerAlive > SERVER_TIMEOUT_ALIVE) {
		return FALSE;
	}

	return TRUE;
}

static void selectClientSocket()
{
	fd_set readfds;
	struct timeval tv;
	int max_fd;
	int sock;
	bool_t isNext;

	max_fd = 0;
	sock = 0;

	if (sock_server_udp != NULL) {
		if (isServerAlive() == FALSE) {
			errorWithServer();
			return;
		}
	}

	do {
		isNext = FALSE;

		tv.tv_sec = 0;
		tv.tv_usec = 0;

		FD_ZERO(&readfds);

		if (sock_server_udp != NULL) {
			sock = sock_server_udp->sock;
		}

		FD_SET(sock, &readfds);
		max_fd = sock;
		select(max_fd + 1, &readfds, (fd_set *) NULL, (fd_set *) NULL, &tv);

		if (FD_ISSET(sock, &readfds)) {
			if (server_eventSelect() > 0) {
				isNext = TRUE;
			}
		}

	} while (isNext == TRUE);
}

#ifdef SUPPORT_TRAFFIC
static void eventTraffic()
{
	my_time_t currentTime;

	currentTime = timer_get_current_time();

	if (currentTime - lastTraffic > 5000) {
		lastTraffic = currentTime;

		DEBUG_MSG(_("[Debug] Traffic: down [%d]/up [%d]\n"), traffic_down, traffic_up);

		traffic_down = 0;
		traffic_up = 0;
	}
}
#endif /* SUPPORT_TRAFFIC */

void client_event()
{
#ifdef SUPPORT_TRAFFIC
	eventTraffic();
#endif /* SUPPORT_TRAFFIC */

	eventPingServer();
	selectClientSocket();
	client_eventWorkRecvList();

}

static void quitUdpClient()
{
	assert(sock_server_udp != NULL);
	sock_udp_close(sock_server_udp);

	DEBUG_MSG(_("[Debug] Closing connection to game server\n"));
}

void client_quit()
{
	proto_send_end_client();

	assert(listRecvMsg != NULL);

	list_destroy_item(listRecvMsg, free);
	buffer_destroy(clientRecvBuffer);
	buffer_destroy(clientSendBuffer);

	if (sock_server_udp != NULL) {
		quitUdpClient();
	}
}
