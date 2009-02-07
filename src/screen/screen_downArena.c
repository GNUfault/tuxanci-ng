
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

#include "screen_setting.h"
#include "screen_downArena.h"
#include "screen_gameType.h"

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
	//printf("status -> %s\n", s);
	destroyWidgetLabel(label_status);
	label_status = newWidgetLabel(s, WINDOW_SIZE_X / 2, 250, WIDGET_LABEL_CENTER);
}

static void connectToDownServer()
{
	char status[STR_SIZE];
	char msg[STR_PROTO_SIZE];

	sock_server_tcp = connectTcpSocket(getSettingIP(), getSettingPort(), PROTO_TCPv4);

	if( sock_server_tcp == NULL )
	{
		sprintf(status, "I do not connect to %s %d TCP down server", getSettingIP(), getSettingPort());
		setStatusString(status);
		return;
	}

	setTcpSockNonBlock(sock_server_tcp);

	recvBuffer = newBuffer(DOWN_ARENA_BUFFER_SIZE);
	sendBuffer = newBuffer(DOWN_ARENA_BUFFER_SIZE);
	status_tcp_socket = DOWN_SERVER_STATUS_OK;

	file = NULL;
	countSendMsg = 0;
	timeSendMsg = getMyTime();

	sprintf(msg, "GETARENA %s\n", arenaNetName);

	addBuffer(sendBuffer, msg, strlen(msg));

	sprintf(status, "Connect to %s %d TCP down server", getSettingIP(), getSettingPort());
	setStatusString(status);
}

static void closeConnectFromDownServer()
{
	closeTcpSocket(sock_server_tcp);
	destroyBuffer(recvBuffer);
	destroyBuffer(sendBuffer);
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
		sprintf(status, "Game server %s %d is down", getSettingIP(), getSettingPort());
		setStatusString(status);
		return;
	}

	timeSendMsg = getMyTime();
	countSendMsg++;
	printf("countSendMsg = %d\n", countSendMsg++);

	if( writeUdpSocket(sock_server_udp, sock_server_udp, msg, strlen(msg)) < 0 )
	{
		char status[STR_SIZE];
		sprintf(status, "I do not send message to %s %d UDP game server", getSettingIP(), getSettingPort());
		setStatusString(status);
	}
}

void startScreenDownArena()
{
	char status[STR_SIZE];

	strcpy(str_status, "none");
	memset(arenaNetName, 0, STR_SIZE);

	sock_server_udp = connectUdpSocket(getSettingIP(), getSettingPort(), PROTO_UDPv4);

	if (sock_server_udp == NULL)
	{
		sprintf(status, "I do not connect to %s %d UDP game server", getSettingIP(), getSettingPort());
		setStatusString(status);
		return;
	}

	setUdpSockNonBlock(sock_server_udp);

	sprintf(status, "Connect to %s %d UDP game server", getSettingIP(), getSettingPort());
	setStatusString(status);

	sendStatusToGameServer();
}

void drawScreenDownArena()
{
	drawWidgetImage(image_backgorund);
	drawWidgetLabel(label_status);
	drawWidgetButton(button_back);
}

static void proto_ok(char *line)
{
	//printf("line = %s\n", line);
	fileSize = atoi(line + 3);
	fileOffset = 0;

	sprintf(path, "%s/%s.zip", getHomeDirector(), arenaNetName);
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

	len = getBufferSize(recvBuffer);
	data = getBufferData(recvBuffer);
	cutBuffer(recvBuffer, len);

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

	ret = readTcpSocket(sock_server_tcp, buffer, STR_PROTO_SIZE - 1);
	//if (ret <= 0) printf("readTcpSocket = %d\n", ret);

	if (ret < 0) {
		return -1;
	}

	if (addBuffer(recvBuffer, buffer, ret) < 0) {
		char status[STR_SIZE];
		sprintf(status, "Error -- recv buffer is full");
		setStatusString(status);
		closeConnectFromDownServer();
		return -1;
	}

	if (file == NULL && getBufferLine(recvBuffer, line, STR_PROTO_SIZE) >= 0) {
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

	len = getBufferSize(sendBuffer);

	if (len == 0) {
		return -1;
	}

	data = getBufferData(sendBuffer);

	res = writeTcpSocket(sock_server_tcp, data, len);

	if (res < 0) {
		return -1;
	}

	cutBuffer(sendBuffer, res);
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
			closeUdpSocket(sock_server_udp);
			sock_server_udp = NULL;
			strncpy(arenaNetName, offset_begin, offset_end - offset_begin);

			if (getArenaFileFormNetName(arenaNetName) != NULL) {
				setScreen("world");
				return;
			}

			connectToDownServer();

			printf("arenaNetName = >>%s<<\n", arenaNetName);
		}
	}
}

static void eventStatus()
{
	if( getMyTime() - timeSendMsg > DOWN_ARENA_MAX_TIEMOUT_LIMIT )
	{
		sendStatusToGameServer();
	}

	if( readUdpSocket(sock_server_udp, sock_server_udp, str_status, STR_PROTO_SIZE - 1) > 0 )
	{
		//printf("str_status = %s\n", str_status);
		readArenaFromStatus();
	}
}

void eventScreenDownArena()
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
		loadArenaFile(path);
		setScreen("world");
	}

	eventWidgetButton(button_back);
}

void stopScreenDownArena()
{
	if (file != NULL) {
		unlink(path);
	}

	if (sock_server_tcp != NULL) {
		closeConnectFromDownServer();
	}

	if (sock_server_udp != NULL) {
		closeUdpSocket(sock_server_udp);
	}

}

static void eventWidget(void *p)
{
	widget_t *button;

	button = (widget_t *) (p);

	if (button == button_back) {
		setScreen("mainMenu");
	}
}

void initScreenDownArena()
{
	image_t *image;

	image = getImage(IMAGE_GROUP_BASE, "screen_main");
	image_backgorund = newWidgetImage(0, 0, image);

	button_back =
		newWidgetButton(_("back"), WINDOW_SIZE_X / 2 - WIDGET_BUTTON_WIDTH / 2,
				WINDOW_SIZE_Y - 80, eventWidget);

	label_status =
		newWidgetLabel("none", WINDOW_SIZE_X / 2, 250, WIDGET_LABEL_CENTER);

	registerScreen( newScreen("downArena", startScreenDownArena, eventScreenDownArena,
			drawScreenDownArena, stopScreenDownArena));
}

void quitScreenDownArena()
{
	destroyWidgetImage(image_backgorund);
	destroyWidgetButton(button_back);
	destroyWidgetLabel(label_status);
}
