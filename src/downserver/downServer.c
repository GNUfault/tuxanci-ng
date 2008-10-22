
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>

#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/stat.h>

#include "list.h"
#include "tcp.h"
#include "buffer.h"
#include "textFile.h"

#define CLIENT_LIMIT_BUFFER	(4096+256)

#define STR_PATH_SIZE			4096
#define STR_ARENA_NET_NAME_SIZE		256
#define BUFFER_SEND_FILE_SIZE 4096

#define NET_STATUS_OK		0
#define NET_STATUS_ZOMBIE	1

#define BUFFER_SIZE		256
#define PROTO_SIZE		256

static sock_tcp_t *sock_server_tcp;
static list_t *listClient;
static textFile_t *configFile;

typedef struct
{
    int status;

    FILE *file;
    int fileSize;
    int fileOffset;

    sock_tcp_t *socket;
    buffer_t *recvBuffer;
    buffer_t *sendBuffer;
} client_t;

static client_t *
newClient(sock_tcp_t * sock)
{
    client_t *new;

    new = malloc(sizeof(client_t));
    new->socket = sock;
    new->status = NET_STATUS_OK;
    new->file = NULL;
    new->recvBuffer = newBuffer(CLIENT_LIMIT_BUFFER);
    new->sendBuffer = newBuffer(CLIENT_LIMIT_BUFFER);

    return new;
}

static void
destroyClient(client_t * client)
{
    if (client->file != NULL)
        fclose(client->file);
    closeTcpSocket(client->socket);
    destroyBuffer(client->recvBuffer);
    destroyBuffer(client->sendBuffer);
    free(client);
}

int
getFileSize(char *s)
{
    struct stat buf;

    if (lstat(s, &buf) < 0) {
        return -1;
    }

    return buf.st_size;
}

static char *
getFileFromArenaNetName(char *s)
{
    int i;

    for (i = 0; i < configFile->text->count; i++) {
        char path[STR_PATH_SIZE];
        char arena[STR_ARENA_NET_NAME_SIZE];
        char *line;
        int len;

        line = configFile->text->list[i];

        sscanf(line, "%s %s", path, arena);
        len = strlen(arena);

        if (strncmp(arena, s, len) == 0) {
            return strdup(path);
        }
    }

    return NULL;
}

static void
proto_getarena(client_t * client, char *msg)
{
    char msg_out[PROTO_SIZE];
    char *path;

    path = getFileFromArenaNetName(msg);

    if (path != NULL) {
        client->file = fopen(path, "rb");
        client->fileSize = getFileSize(path);
        client->fileOffset = 0;
        free(path);

        if (client->file == NULL || client->fileSize < 0) {
            client->file = NULL;
            client->fileSize = 0;
            sprintf(msg_out, "ERR server has a problem !\n");
        }
        else {
            sprintf(msg_out, "OK %d\n", client->fileSize);
        }
    }
    else {
        sprintf(msg_out, "ERR arena not found\n");
    }

    addBuffer(client->sendBuffer, msg_out, strlen(msg_out));
}

static void
doProto(client_t * client, char *msg)
{
    printf("hmm msg = %s\n", msg);

    if (strncmp(msg, "GETARENA", 8) == 0 && client->file == NULL) {
        proto_getarena(client, msg + 9);
    }
}

static int
initDownServer(char *ip, int port)
{
    sock_server_tcp = bindTcpSocket(ip, port, PROTO_TCPv4);

    listClient = newList();

    configFile = loadTextFile("arena.conf");

    return 0;
}

static void
eventNewClient(sock_tcp_t * server_sock)
{
    sock_tcp_t *sock;
    client_t *client;

    sock = getTcpNewClient(server_sock);
    setTcpSockNonBlock(sock);

    client = newClient(sock);
    addList(listClient, client);
}

static void
eventRecvClient(client_t * client)
{
    char buffer[BUFFER_SIZE];
    char line[PROTO_SIZE];
    int ret;

    memset(buffer, 0, BUFFER_SIZE);

    ret = readTcpSocket(client->socket, buffer, BUFFER_SIZE - 1);
    //printf("readTcpSocket = %d\n", ret);

    if (ret <= 0) {
        client->status = NET_STATUS_ZOMBIE;
        return;
    }

    if (addBuffer(client->recvBuffer, buffer, ret) < 0) {
        client->status = NET_STATUS_ZOMBIE;
        return;
    }

    while (getBufferLine(client->recvBuffer, line, PROTO_SIZE) >= 0) {
        doProto(client, line);
    }
}

static void
eventSendClient(client_t * client)
{
    void *data;
    int len;
    int res;

    len = getBufferSize(client->sendBuffer);

    if (len == 0) {
        return;
    }

    data = getBufferData(client->sendBuffer);

    res = writeTcpSocket(client->socket, data, len);
    //printf("send = %d\n", res);
    //printf("writeTcpSocket = %d\n", res);

    if (res < 0) {
        client->status = NET_STATUS_ZOMBIE;
        return;
    }

    cutBuffer(client->sendBuffer, res);
}

static void
eventClient(client_t * client)
{
    if (client->file != NULL && getBufferSize(client->sendBuffer) == 0) {
        char buffer[BUFFER_SEND_FILE_SIZE];
        int count;
        int ret;

        count = client->fileSize - client->fileOffset;

        if (count > BUFFER_SEND_FILE_SIZE) {
            count = BUFFER_SEND_FILE_SIZE;
        }

        ret = fread(buffer, count, 1, client->file);
        //printf("read = %d\n", count);

        if (ret == 1) {
            if (addBuffer(client->sendBuffer, buffer, count) < 0) {
                client->status = NET_STATUS_ZOMBIE;
                return;
            }

            client->fileOffset += count;
        }
        else {
            client->status = NET_STATUS_ZOMBIE;
        }
    }
}

void
startDownServer()
{
    struct timeval tv;
    fd_set readfds;
    fd_set writefds;
    int max_fd;
    int ret;
    int i;

    tv.tv_sec = 1;
    tv.tv_usec = 0;
    FD_ZERO(&readfds);
    FD_ZERO(&writefds);

    max_fd = sock_server_tcp->sock;

    FD_SET(sock_server_tcp->sock, &readfds);

    for (i = 0; i < listClient->count; i++) {
        client_t *client;
        int sock;

        client = (client_t *) listClient->list[i];

        if (client->status != NET_STATUS_ZOMBIE) {
            sock = client->socket->sock;

            if (sock > max_fd) {
                max_fd = sock;
            }

            FD_SET(sock, &readfds);

            if (getBufferSize(client->sendBuffer) > 0) {
                FD_SET(sock, &writefds);
            }
        }
    }

    ret = select(max_fd + 1, &readfds, &writefds, (fd_set *) NULL, &tv);

    if (ret > 0) {
        if (FD_ISSET(sock_server_tcp->sock, &readfds)) {
            eventNewClient(sock_server_tcp);
        }

        for (i = 0; i < listClient->count; i++) {
            client_t *client;
            int sock;

            client = (client_t *) listClient->list[i];
            sock = client->socket->sock;

            if (FD_ISSET(sock, &readfds)) {
                eventRecvClient(client);
            }

            if (FD_ISSET(sock, &writefds)) {
                eventSendClient(client);
            }
        }
    }

    for (i = 0; i < listClient->count; i++) {
        client_t *client;

        client = (client_t *) listClient->list[i];

        if (client->status == NET_STATUS_ZOMBIE) {
            delListItem(listClient, i, destroyClient);
            i--;
            continue;
        }

        eventClient(client);
    }
}

void
quitDownServer()
{
    closeTcpSocket(sock_server_tcp);
    destroyListItem(listClient, destroyClient);
}

int
isFillPath(const char *path)
{
    return 0;
}

int
main(int argc, char **argv)
{
    signal(SIGPIPE, SIG_IGN);

    initDownServer("127.0.0.1", 2200);

    while (1) {
        startDownServer();
    }
}
