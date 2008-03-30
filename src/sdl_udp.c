

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

#include "main.h"
#include "sdl_udp.h"

#define BUFSIZE 1000


sock_sdl_udp_t* newSdlSockUdp()
{
	sock_sdl_udp_t *new;

	new = malloc( sizeof(sock_sdl_udp_t) );
	memset(new, 0, sizeof(sock_sdl_udp_t));

	if( !( new->packet = SDLNet_AllocPacket(512) ) )
	{
		fprintf(stderr, "nemozem alokovat pamat pre packet !\n");
		destroySockSdlUdp(new);
	}

	return new;
}

void destroySockSdlUdp(sock_sdl_udp_t *p)
{
	assert( p != NULL );

	SDLNet_FreePacket(p->packet);
	free(p);
}

sock_sdl_udp_t* bindSdlUdpSocket(int port)
{
	sock_sdl_udp_t *new;

	assert( port > 0 && port < 65535 );

	new = newSdlSockUdp();

	assert( new != NULL );

	if( SDLNet_Init() < 0 )
	{
		fprintf(stderr, "nemozem initcializovat SDLnet !\n");
		destroySockSdlUdp(new);
		return NULL;
	}
 
	if( !( new->sock = SDLNet_UDP_Open(port) ) )
	{
		fprintf(stderr, "nemozem otovrit socket !\n");
		destroySockSdlUdp(new);
		return NULL;
	}

	new->type = SDL_UDP_SERVER;

	return new;
}

sock_sdl_udp_t* connectSdlUdpSocket(char *address, int port)
{
	sock_sdl_udp_t *new;

	assert( address != NULL  );
	assert( port > 0 && port < 65536 );

	new = newSdlSockUdp();

	assert( new != NULL );

	/* Initialize SDL_net */
	if( SDLNet_Init() < 0 )
	{
		fprintf(stderr, "nemozem initcializovat SDLnet !\n");
		destroySockSdlUdp(new);
		return NULL;
	}
	
	/* Open a socket on random port */
	if( !( new->sock = SDLNet_UDP_Open(0) ) )
	{
		fprintf(stderr, "nemozem otovrit socket !\n");
		destroySockSdlUdp(new);
		return NULL;
	}
 
	/* Resolve server name  */
	if( SDLNet_ResolveHost(&(new->srvadd), address, port) )
	{
		fprintf(stderr, "nemozem sa pripojit na sokcet !\n");
		destroySockSdlUdp(new);
		return NULL;
	}

	new->type = SDL_UDP_CLIENT;

	return new;
}

int readSdlUdpSocket(sock_sdl_udp_t *src, sock_sdl_udp_t *dst, void *address, int len)
{
	int x;

	if( SDLNet_UDP_Recv(src->sock, dst->packet) )
	{
		x = (dst->packet->len > len ? len : dst->packet->len);
		memcpy(address, dst->packet->data, x);
		return x;
	}
 
	return -1;
}

int writeSdlUdpSocket(sock_sdl_udp_t *src, sock_sdl_udp_t *dst, void *address, int len)
{
	assert( src != NULL );
	assert( address != NULL );

#ifdef DEBUG_LOST_PACKET
	if( rand() % DEBUG_LOST_PACKET == 0 )
	{
		printf("debug lost packet %s\n", address);
		return size;
	}
#endif

	memcpy(dst->packet->data, address, len);
	dst->packet->len = len;

	if( src->type == SDL_UDP_CLIENT )
	{
		dst->packet->address.host = src->srvadd.host;
		dst->packet->address.port = src->srvadd.port;
 	}

	SDLNet_UDP_Send(src->sock, -1, dst->packet);
 
	return len;
}

int getSockSdlUdpPort(sock_sdl_udp_t *p)
{
	assert( p != NULL );

	return p->packet->address.port;
}


void closeSdlUdpSocket(sock_sdl_udp_t *p)
{
	assert( p != NULL  );

	SDLNet_UDP_Close(p->sock);

	printf("close connect\n");

	destroySockSdlUdp(p);
}

#if 0
int main(int argc, char *argv[])
{
	sock_sdl_udp_t *sock;
	char buf[BUFSIZE];
	int ret;

	if( strcmp(argv[1], "s") == 0 )
	{
		sock_sdl_udp_t *cli;

		sock = bindSdlUdpSocket( atoi(argv[2]) );
		cli = newSdlSockUdp();
	
		while(1)
		{
			memset(buf, 0, BUFSIZE);
			
			ret = readSdlUdpSocket(sock, cli, buf, BUFSIZE);
			
			if( ret > 0 )
			{
				writeSdlUdpSocket(sock, cli, buf, ret);
			}
		}
	}

	if( strcmp(argv[1], "c") == 0 )
	{
		sock = connectSdlUdpSocket("127.0.0.1", atoi(argv[2]));
	
		strcpy(buf, "Hello world !\n");
		writeSdlUdpSocket(sock, sock, buf, strlen(buf));

		while(1)
		{
			memset(buf, 0, BUFSIZE);
			ret = readSdlUdpSocket(sock, sock, buf, BUFSIZE);

			if( ret > 0 )
			{
				printf("buf = %s", buf);
			}
		}

		closeSdlUdpSocket(sock);
	}

	return 0;
}

#endif
