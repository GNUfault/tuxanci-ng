
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

#include "main.h"
#include "list.h"
#include "buffer.h"
#include "textFile.h"
#include "arenaFile.h"
#include "homeDirector.h"

#include "interface.h"
#include "screen.h"
#include "image.h"

#include "net_multiplayer.h"
#include "udp.h"
#include "tcp.h"

#include "widget.h"
#include "widget_image.h"
#include "widget_label.h"
#include "widget_button.h"

#include "setting.h"
#include "downArena.h"
#include "gameType.h"

static sock_udp_t *sock_server_udp;
static sock_tcp_t *sock_server_tcp;

static widget_t *image_backgorund;
static widget_t *button_back;
static widget_t *label_status;

static char str_status[STR_PROTO_SIZE];
static char arenaNetName[STR_SIZE];

static int status_tcp_socket;
static buffer_t *recvBuffer;
static buffer_t *sendBuffer;

static char path[STR_PATH_SIZE];
static FILE *file;
static int fileSize;
static int fileOffset;

static my_time_t timeSendMsg;
static int countSendMsg;

static void setStatusString(char *s)
{
	printf("status -> %s\n", s);
	label_destroy(label_status);
	label_status = label_new(s, WINDOW_SIZE_X / 2, 250, WIDGET_LABEL_CENTER);
}

static void connectToDownServer()
{
	char status[STR_SIZE];
	char msg[STR_PROTO_SIZE];

	sock_server_tcp = sock_tcp_connect(publicServer_get_settingIP(), publicServer_get_settingPort());

	if( sock_server_tcp == NULL )
	{
		sprintf(status, "I do not connect to %s %d TCP down server", publicServer_get_settingIP(), publicServer_get_settingPort());
		setStatusString(status);
		return;
	}

	sock_tcp_set_non_block(sock_server_tcp);

	recvBuffer = buffer_new(DOWN_ARENA_BUFFER_SIZE);
	sendBuffer = buffer_new(DOWN_ARENA_BUFFER_SIZE);
	status_tcp_socket = DOWN_SERVER_STATUS_OK;

	file = NULL;
	countSendMsg = 0;
	timeSendMsg = timer_get_current_time();

	sprintf(msg, "GETARENA %s\n", arenaNetName);

	buffer_append(sendBuffer, msg, strlen(msg));

	sprintf(status, "Connect to %s %d TCP down server", publicServer_get_settingIP(), publicServer_get_settingPort());
	setStatusString(status);
}

static void closeConnectFromDownServer()
{
	sock_tcp_close(sock_server_tcp);
	buffer_destroy(recvBuffer);
	buffer_destroy(sendBuffer);
	sock_server_tcp = NULL;
	recvBuffer = NULL;
	sendBuffer = NULL;
}

static void sendStatusToGameServer()
{
	char *msg = "status\n";

	if( countSendMsg > DOWN_ARENA_MAX_SEND_MSG_STATUS )
	{
		char status[STR_SIZE];
		sprintf(status, "Game server %s %d is down", publicServer_get_settingIP(), publicServer_get_settingPort());
		setStatusString(status);
		return;
	}

	timeSendMsg = timer_get_current_time();
	countSendMsg++;
	printf("countSendMsg = %d\n", countSendMsg++);

	if( sock_udp_write(sock_server_udp, sock_server_udp, msg, strlen(msg)) < 0 )
	{
		char status[STR_SIZE];
		sprintf(status, "I do not send message to %s %d UDP game server", publicServer_get_settingIP(), publicServer_get_settingPort());
		setStatusString(status);
	}
}

void downArena_start()
{
	char status[STR_SIZE];

	strcpy(str_status, "none");
	memset(arenaNetName, 0, STR_SIZE);

	sock_server_udp = sock_udp_connect(publicServer_get_settingIP(), publicServer_get_settingPort());

	if (sock_server_udp == NULL)
	{
		sprintf(status, "I do not connect to %s %d UDP game server", publicServer_get_settingIP(), publicServer_get_settingPort());
		setStatusString(status);
		return;
	}

	sock_udp_set_non_block(sock_server_udp);

	sprintf(status, "Connect to %s %d UDP game server", publicServer_get_settingIP(), publicServer_get_settingPort());
	setStatusString(status);

	sendStatusToGameServer();
}

void downArena_draw()
{
	wid_image_draw(image_backgorund);
	label_draw(label_status);
	button_draw(button_back);
}

static void proto_ok(char *line)
{
	//printf("line = %s\n", line);
	fileSize = atoi(line + 3);
	fileOffset = 0;

	sprintf(path, "%s/%s.zip", homeDirector_get(), arenaNetName);
	//printf("path = %s\n", path);
	file = fopen(path, "wb");
}

static void proto_err(char *line)
{
	//printf("line = %s\n", line);
	setStatusString(line + 4);
	closeConnectFromDownServer();
}

static void eventProto(char *line)
{
	if( strncmp(line, "OK", 2) == 0 )
	{
		proto_ok(line);
	}

	if( strncmp(line, "ERR", 3) == 0 )
	{
		proto_err(line);
	}
}

static int downArenaFile()
{
	char status[STR_SIZE];
	void *data;
	int len;

	len = buffer_get_size(recvBuffer);
	data = buffer_get_data(recvBuffer);
	buffer_cut(recvBuffer, len);

	fwrite(data, len, 1, file);
	fileOffset += len;

	sprintf(status, "I down %d / %d", fileOffset, fileSize);
	setStatusString(status);

	return len;
}

static int eventRecvBuffer()
{
	char buffer[STR_PROTO_SIZE];
	char line[STR_PROTO_SIZE];
	int ret;

	memset(buffer, 0, STR_PROTO_SIZE);

	ret = sock_tcp_read(sock_server_tcp, buffer, STR_PROTO_SIZE - 1);
	//if (ret <= 0) printf("sock_tcp_read = %d\n", ret);

	if (ret < 0) {
		return -1;
	}

	if (buffer_append(recvBuffer, buffer, ret) < 0) {
		char status[STR_SIZE];
		sprintf(status, "Error -- recv buffer is full");
		setStatusString(status);
		closeConnectFromDownServer();
		return -1;
	}

	if (file == NULL && buffer_get_line(recvBuffer, line, STR_PROTO_SIZE) >= 0) {
		eventProto(line);
		return ret;
	}

	if (file != NULL) {
		return downArenaFile();
	}

	return -1;
}

static int eventSendBuffer()
{
	void *data;
	int len;
	int res;

	len = buffer_get_size(sendBuffer);

	if (len == 0) {
		return -1;
	}

	data = buffer_get_data(sendBuffer);

	res = sock_tcp_write(sock_server_tcp, data, len);

	if (res < 0) {
		return -1;
	}

	buffer_cut(sendBuffer, res);
	return res;
}

static void readArenaFromStatus()
{
	const char *str_arena = "arena: ";

	char *offset_begin;
	char *offset_end;

	offset_begin = strstr(str_status, str_arena);

	if (offset_begin != NULL) {
		offset_begin += strlen(str_arena);
		offset_end = strstr(offset_begin, "\n");

		if (offset_end != NULL) {
			sock_udp_close(sock_server_udp);
			sock_server_udp = NULL;
			strncpy(arenaNetName, offset_begin, offset_end - offset_begin);

			if (arenaFile_get_file_format_net_name(arenaNetName) != NULL) {
				screen_set("world");
				return;
			}

			connectToDownServer();

			printf("arenaNetName = >>%s<<\n", arenaNetName);
		}
	}
}

static void eventStatus()
{
	if( timer_get_current_time() - timeSendMsg > DOWN_ARENA_MAX_TIEMOUT_LIMIT )
	{
		sendStatusToGameServer();
	}

	if( sock_udp_read(sock_server_udp, sock_server_udp, str_status, STR_PROTO_SIZE - 1) > 0 )
	{
		//printf("str_status = %s\n", str_status);
		readArenaFromStatus();
	}
}

void downArena_event()
{
	int i;

	if (strcmp(str_status, "none") == 0) {
		eventStatus();
	}

	if (recvBuffer != NULL) {
		for (i = 0; i < DOWN_ARENA_COUNT_READ_SOCKET; i++) {
			if (eventRecvBuffer() <= 0) {
				break;
			}
		}
	}

	if (sendBuffer != NULL) {
		eventSendBuffer();
	}

	if (file != NULL && fileSize == fileOffset && fileSize > 0) {
		fclose(file);
		file = NULL;
		closeConnectFromDownServer();
		arenaFile_load(path);
		screen_set("world");
	}

	button_event(button_back);
}

void downArena_stop()
{
	if (file != NULL) {
		unlink(path);
	}

	if (sock_server_tcp != NULL) {
		closeConnectFromDownServer();
	}

	if (sock_server_udp != NULL) {
		sock_udp_close(sock_server_udp);
	}

}

static void eventWidget(void *p)
{
	widget_t *button;

	button = (widget_t *) (p);

	if (button == button_back) {
		screen_set("mainMenu");
	}
}

void downArena_init()
{
	image_t *image;

	image = image_get(IMAGE_GROUP_BASE, "screen_main");
	image_backgorund = wid_image_new(0, 0, image);

	button_back =
		button_new(_("back"), WINDOW_SIZE_X / 2 - WIDGET_BUTTON_WIDTH / 2,
				WINDOW_SIZE_Y - 80, eventWidget);

	label_status =
		label_new("none", WINDOW_SIZE_X / 2, 250, WIDGET_LABEL_CENTER);

	screen_register( screen_new("downArena", downArena_start, downArena_event,
			downArena_draw, downArena_stop));
}

void downArena_quit()
{
	wid_image_destroy(image_backgorund);
	button_destroy(button_back);
	label_destroy(label_status);
}
