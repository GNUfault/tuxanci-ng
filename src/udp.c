

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <netdb.h>
#include <assert.h>

#define BUFSIZE 1000

#include "main.h"

typedef struct struct_sock_udp_t
{
	int sock;
	struct sockaddr_in sockAddr;
} sock_udp_t;

sock_udp_t* newSockUdp()
{
	sock_udp_t *new;

	new = malloc( sizeof(sock_udp_t) );
	memset(new, 0, sizeof(sock_udp_t));

	return new;
}

void destroySockUdp(sock_udp_t *p)
{
	assert( p != NULL );
	free(p);
}

sock_udp_t* bindUdpSocket(char *address, int port)
{
	sock_udp_t *new;

	assert( port > 0 && port < 65535 );

	new = newSockUdp();

	assert( new != NULL );

	if( ( new->sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP) ) < 0 )
	{
		fprintf(stderr, "nemozem obsadit sokcet !\n");
		destroySockUdp(new);
		return NULL;
	}

	new->sockAddr.sin_family = AF_INET;
	new->sockAddr.sin_port = htons(port);
	new->sockAddr.sin_addr.s_addr = inet_addr(address);

	if( bind(new->sock, (struct sockaddr *)&(new->sockAddr), sizeof(new->sockAddr)) < 0 )
	{
		fprintf(stderr, "nemozem nastavit sokcet !\n");
		destroySockUdp(new);
		return NULL;
	}

	return new;
}

sock_udp_t* connectUdpSocket(char *address, int port)
{
	struct hostent *host;
	sock_udp_t *new;

	assert( address != NULL  );
	assert( port > 0 && port < 65536 );

	new = newSockUdp();

	assert( new != NULL );

	if( ( new->sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP) ) < 0 )
	{
		fprintf(stderr, "nemozem sa pripojit na sokcet !\n");
		destroySockUdp(new);
		return NULL;
	}

	host = gethostbyname(address);
	new->sockAddr.sin_family = AF_INET;
	new->sockAddr.sin_port = htons(port);
	memcpy(&(new->sockAddr.sin_addr), host->h_addr, host->h_length);

	return new;
}

int readUdpSocket(sock_udp_t *src, sock_udp_t *dst, void *address, int len)
{
	int addrlen;
	int size;

	assert( src != NULL );
	assert( dst != NULL );
	assert( address != NULL );

        addrlen = sizeof(src->sockAddr);

        if( ( size = recvfrom(src->sock, address, len, 0,
		(struct sockaddr *)&dst->sockAddr, (socklen_t *)&addrlen) ) < 0 )
        {
		fprintf(stderr, "nemozem nacitat sokcet !\n");
		return -1;
        }

	return size;
}

int writeUdpSocket(sock_udp_t *src, sock_udp_t *dst, void *address, int len)
{
	int addrlen;
	int size;

#ifdef DEBUG_LOST_PACKET
	if( rand() % DEBUG_LOST_PACKET == 0 )
	{
		printf("debug lost packet %s\n", address);
		return size;
	}
#endif

        addrlen = sizeof(src->sockAddr);

	assert( src != NULL );
	assert( dst != NULL );
	assert( address != NULL );

       if( ( size = sendto(src->sock, address, len, 0,
		(struct sockaddr *)&dst->sockAddr, (socklen_t)addrlen) ) < 0 )
        {
		fprintf(stderr, "nemozem nacitat sokcet !\n");
		return -1;
        }

	return size;
}

void getSockUdpIp(sock_udp_t *p, char *str_ip)
{
	assert( p != NULL  );
	assert( str_ip != NULL );

	strcpy(str_ip, inet_ntoa(p->sockAddr.sin_addr));
}

int getSockUdpPort(sock_udp_t *p)
{
	assert( p != NULL );
	return htons(p->sockAddr.sin_port);
}


void closeUdpSocket(sock_udp_t *p)
{
	char str_ip[STR_IP_SIZE];

	assert( p != NULL  );

	getSockUdpIp(p, str_ip);
	printf("close connect from %s:%d\n", str_ip, htons(p->sockAddr.sin_port));

	close(p->sock);
	destroySockUdp(p);
}

#if 0

static int myWait(sock_udp_t *p)
{
	fd_set readfds;
	fd_set errorfds;
	struct timeval tv;
	int max_fd;

	tv.tv_sec = 1;
	tv.tv_usec = 0;
	
	FD_ZERO(&readfds);
	FD_ZERO(&errorfds);
	max_fd = p->sock;

	FD_SET(p->sock, &readfds);
	FD_SET(p->sock, &errorfds);

	select(max_fd+1, &readfds, (fd_set *)0, &errorfds, &tv);

	if( FD_ISSET(p->sock, &readfds) )
	{
		FD_CLR(p->sock, &readfds);
		return 1;
	}

	return 0;
}

int test_udp_main(int argc, char *argv[])
{
	sock_udp_t *sock;
	char buf[BUFSIZE];
	int ret;

	if( strcmp(argv[1], "s") == 0 )
	{
		sock_udp_t *cli;

		sock = bindUdpSocket( atoi(argv[2]) );
		cli = newSockUdp();
	
		while(1)
		{
			while( myWait(sock) == 0 )
				;

			memset(buf, 0, BUFSIZE);
			ret = readUdpSocket(sock, cli, buf, BUFSIZE);
			writeUdpSocket(sock, cli, buf, ret);
		}
	}

	if( strcmp(argv[1], "c") == 0 )
	{
		sock = connectUdpSocket("127.0.0.1", atoi(argv[2]));
	
		strcpy(buf, "Hello world !\n");
		writeUdpSocket(sock, sock, buf, strlen(buf));

		memset(buf, 0, BUFSIZE);
		readUdpSocket(sock, sock, buf, BUFSIZE);

		printf("buf = %s", buf);
		
		closeUdpSocket(sock);
	}

	return 0;
}
#endif

