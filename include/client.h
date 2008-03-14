
#ifndef MY_CLIENT

#define MY_CLIENT

#define CLIENT_TIMEOUT	2500

extern int initTcpClient(char *ip, int port);
extern int initUdpClient(char *ip, int port);
extern void sendServer(char *msg);
extern void eventServerBuffer();
extern void selectClientTcpSocket();
extern void eventPingServer();
extern void selectClientUdpSocket();
extern void quitTcpClient();
extern void quitUdpClient();

#endif 
