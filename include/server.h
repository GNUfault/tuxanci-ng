
#ifndef MY_SERVER

#define MY_SERVER

#include "list.h"
#include "tux.h"
#include "buffer.h"
#include "tcp.h"
#include "udp.h"

#define SERVER_TIMEOUT	5000

typedef struct client_struct
{
	sock_tcp_t *socket_tcp;
	sock_udp_t *socket_udp;
	int status;
	Uint32 lastPing;
	tux_t *tux;
	buffer_t *buffer;
} client_t;

int initTcpServer(int port);
int initUdpServer(int port);
client_t* newTcpClient(sock_tcp_t *sock_tcp);
client_t* newUdpClient(sock_udp_t *sock_udp);
void destroyClient(client_t *p);
void sendClient(client_t *p, char *msg);
void sendAllClientBut(char *msg, client_t *p);
void sendAllClient(char *msg);
void eventClientListBuffer();
void selectServerTcpSocket();
void selectServerUdpSocket();
void quitTcpServer();
void quitUdpServer();
#endif 
 
