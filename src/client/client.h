
#ifndef MY_CLIENT

#define MY_CLIENT

#include "main.h"


#define CLIENT_TIMEOUT		1000
#define SERVER_TIMEOUT_ALIVE	5000

#define CLIENT_BUFFER_LIMIT	4096

extern int initClient(char *ip, int port, int proto);
extern void sendServer(char *msg);
extern void eventClient();
extern void quitClient();

#endif
