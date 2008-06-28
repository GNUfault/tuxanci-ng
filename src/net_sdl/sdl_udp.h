
#ifndef MY_SDL_UDP

#include <SDL_net.h>

#define MY_SDL_UDP

#define SDL_UDP_SERVER 0
#define SDL_UDP_CLIENT 1

typedef struct struct_sock_sdl_udp_t
{
	int type;
	UDPsocket sock;
	IPaddress srvadd;
	UDPpacket *packet;
} sock_sdl_udp_t;

sock_sdl_udp_t* newSdlSockUdp();
void destroySockSdlUdp(sock_sdl_udp_t *p);
sock_sdl_udp_t* bindSdlUdpSocket(int port);
sock_sdl_udp_t* connectSdlUdpSocket(char *address, int port);
int readSdlUdpSocket(sock_sdl_udp_t *src, sock_sdl_udp_t *dst, void *address, int len);
int writeSdlUdpSocket(sock_sdl_udp_t *src, sock_sdl_udp_t *dst, void *address, int len);
int getSockSdlUdpPort(sock_sdl_udp_t *p);
void closeSdlUdpSocket(sock_sdl_udp_t *p);

#endif
