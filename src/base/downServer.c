#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>

#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/stat.h>

#include "main.h"
#include "list.h"
#include "server.h"
#include "myTimer.h"
#include "arena.h"
#include "serverSelect.h"
#include "downServer.h"
#include "arenaFile.h"

#include "tcp.h"
#include "buffer.h"

#define DOWN_SERVER_LIMIT_BUFFER	10240
#define DOWN_SERVER_SEND_FILE_BUFFER	4096

#define NET_STATUS_OK			0
#define NET_STATUS_ZOMBIE		1

static sock_tcp_t *sock_server_tcp;
static list_t *listClient;

typedef struct client_down_server_struct {
	int status_connect;
	sock_tcp_t *sock;
	buffer_t *recvBuffer;
	buffer_t *sendBuffer;

	FILE *file;
	int size;
	int send;
} client_ds_t;

static int getFileSize(char *s)
{
	struct stat buf;

	if (lstat(s, &buf) < 0) {
		return -1;
	}

	return buf.st_size;
}

static void proto_getarena(client_ds_t *client, char *msg)
{
	arenaFile_t *arenaFile;
	char msg_out[STR_PROTO_SIZE];
	char *arenaName;

	arenaName = msg + 9;

	/*printf("arena = >>%s<<\n", arenaName);*/
	arenaFile = arena_file_get_file_format_net_name(arenaName);

	if (arenaFile == NULL) {
		char *error_msg = _("[Error] Arena not found\n");
		buffer_append(client->sendBuffer, error_msg, strlen(error_msg));
		return;
	}

	client->file = fopen(arenaFile->path, "rb");

	if (client->file == NULL) {
		char *error_msg = _("[Error] Server has some problem\n");
		buffer_append(client->sendBuffer, error_msg, strlen(error_msg));
		return;
	}

	client->size = getFileSize(arenaFile->path);
	sprintf(msg_out, _("OK %d\n"), client->size);
	buffer_append(client->sendBuffer, msg_out, strlen(msg_out));
}

static void do_proto(client_ds_t *client, char *msg)
{
	if (strncmp(msg, "GETARENA ", 9) == 0 && client->file == NULL) {
		proto_getarena(client, msg);
	}
}

static client_ds_t *newClient(sock_tcp_t *sock)
{
	client_ds_t *new;

	new = malloc(sizeof(client_ds_t));
	memset(new, 0, sizeof(client_ds_t));

	new->status_connect = NET_STATUS_OK;
	new->sock = sock;
	new->recvBuffer = buffer_new(DOWN_SERVER_LIMIT_BUFFER);
	new->sendBuffer = buffer_new(DOWN_SERVER_LIMIT_BUFFER);

	return new;
}

static void destroyClient(client_ds_t *client)
{
	sock_tcp_close(client->sock);
	buffer_destroy(client->recvBuffer);
	buffer_destroy(client->sendBuffer);

	if (client->file != NULL) {
		fclose(client->file);
	}

	free(client);
}

int down_server_init(char *ip4, int port4)
{
	sock_server_tcp = sock_tcp_bind(ip4, port4);

	if (sock_server_tcp == NULL) {
		fprintf(stderr, _("[Error] Unable to initialize download server\n"));
		return -1;
	}

	sock_tcp_set_non_block(sock_server_tcp);

	listClient = list_new();

	return 0;
}

int down_server_set_select()
{
	int i;

	select_add_sock_for_read(sock_server_tcp->sock);

	for (i = 0; i < listClient->count; i++) {
		client_ds_t *this;

		this = (client_ds_t *) listClient->list[i];

		select_add_sock_for_read(this->sock->sock);

		if (buffer_get_size(this->sendBuffer) > 0) {
			select_add_sock_for_write(this->sock->sock);
		}
	}

	return 0;
}

static void eventNewClient(sock_tcp_t *sock_server)
{
	sock_tcp_t *sock_client;
	client_ds_t *client;

	sock_client = sock_tcp_accept(sock_server);
	sock_tcp_set_non_block(sock_client);
	client = newClient(sock_client);

	list_add(listClient, client);
}

static void eventReadClient(client_ds_t *client)
{
	char buffer[STR_PROTO_SIZE];
	int ret;

	memset(buffer, 0, STR_PROTO_SIZE);

	ret = sock_tcp_read(client->sock, buffer, STR_PROTO_SIZE - 1);
	/*printf("sock_tcp_read = %d\n", ret);*/

	if (ret <= 0) {
		client->status_connect = NET_STATUS_ZOMBIE;
		return;
	}

	if (buffer_append(client->recvBuffer, buffer, ret) < 0) {
		client->status_connect = NET_STATUS_ZOMBIE;
		return;
	}

	while (buffer_get_line(client->recvBuffer, buffer, STR_PROTO_SIZE) >= 0) {
		int len;

		len = strlen(buffer);

		if (buffer[len - 1] == '\n') {
			buffer[len - 1] = '\0';
		}

		do_proto(client, buffer);
		/*buffer_append(client->sendBuffer, buffer, strlen(buffer));*/
	}
}

static void eventWriteClient(client_ds_t *client)
{
	void *data;
	int len;
	int res;

	len = buffer_get_size(client->sendBuffer);

	if (len == 0) {
		return;
	}

	data = buffer_get_data(client->sendBuffer);

	res = sock_tcp_write(client->sock, data, len);
	/*printf("sock_tcp_write = %d\n", res);*/

	if (res < 0) {
		client->status_connect = NET_STATUS_ZOMBIE;
		return;
	}

	buffer_cut(client->sendBuffer, res);
}

static void sendFile(client_ds_t *client)
{
	char buffer[DOWN_SERVER_SEND_FILE_BUFFER];
	int count;

	count = client->size - client->send;

	if (count > DOWN_SERVER_SEND_FILE_BUFFER) {
		count = DOWN_SERVER_SEND_FILE_BUFFER;
	}

	if (fread(buffer, count, 1, client->file) != 1) {
		client->status_connect = NET_STATUS_ZOMBIE;
		return;
	}

	buffer_append(client->sendBuffer, buffer, count);
	client->send += count;
}

int down_server_select_socket()
{
	int i;

	if (select_is_change_sock_for_read(sock_server_tcp->sock)) {
		eventNewClient(sock_server_tcp);
	}

	for (i = 0; i < listClient->count; i++) {
		client_ds_t *this;

		this = (client_ds_t *) listClient->list[i];

		if (select_is_change_sock_for_read(this->sock->sock)) {
			eventReadClient(this);
		}

		if (select_is_change_sock_for_write(this->sock->sock)) {
			eventWriteClient(this);
		}

		if (this->file != NULL && buffer_get_size(this->sendBuffer) == 0) {
			sendFile(this);
		}

		if (this->status_connect == NET_STATUS_ZOMBIE) {
			list_del_item(listClient, i, destroyClient);
		}
	}

	return 0;
}

int down_server_quit()
{
	sock_tcp_close(sock_server_tcp);
	list_destroy_item(listClient, destroyClient);

	return 0;
}
