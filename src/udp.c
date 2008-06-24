

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <assert.h>
#include <netdb.h>

#define BUFSIZE 1000
#include "udp.h"
#include "main.h"

sock_udp_t* newSockUdp(int proto)
{
	sock_udp_t *new;

	new = malloc( sizeof(sock_udp_t) );
	memset(new, 0, sizeof(sock_udp_t));
	new->proto = proto;

	return new;
}

void destroySockUdp(sock_udp_t *p)
{
	assert( p != NULL );
	free(p);
}

sock_udp_t* bindUdpSocket(char *address, int port, int proto)
{
	sock_udp_t *new;
	int res;

	assert( port > 0 && port < 65535 );

	new = newSockUdp(proto);

	assert( new != NULL );

	if( new->proto == PROTO_UDPv4 )
	{
		new->sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	}

#ifdef SUPPORT_IPv6
	if( new->proto == PROTO_UDPv6 )
	{
		new->sock = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);
	}
#endif

	if( new->sock < 0 )
	{
		fprintf(stderr, "nemozem vytvorit socket !\n");
		destroySockUdp(new);
		return NULL;
	}

	memset(&(new->sockAddr), 0, sizeof(new->sockAddr));

	if( new->proto == PROTO_UDPv4 )
	{
		new->sockAddr.sin_family = AF_INET;
		new->sockAddr.sin_port = htons(port);
		new->sockAddr.sin_addr.s_addr = inet_addr(address);

		res = bind(new->sock, (struct sockaddr *)&(new->sockAddr), sizeof(new->sockAddr));
	}

#ifdef SUPPORT_IPv6
	if( new->proto == PROTO_UDPv6 )
	{
		new->sockAddr6.sin6_family = AF_INET6;
		new->sockAddr6.sin6_port = htons(port);
		inet_pton(AF_INET6, address, &(new->sockAddr6.sin6_addr));

		res = bind(new->sock, (struct sockaddr *)&(new->sockAddr6), sizeof(new->sockAddr6));
	}
#endif

	if(  res < 0 )
	{
		fprintf(stderr, "nemozem nastavit sokcet %s %d !\n", address, port);
		destroySockUdp(new);
		return NULL;
	}

	return new;
}

sock_udp_t* connectUdpSocket(char *address, int port, int proto)
{
	sock_udp_t *new;

	assert( address != NULL  );
	assert( port > 0 && port < 65536 );

	new = newSockUdp(proto);

	assert( new != NULL );

	if( new->proto == PROTO_UDPv4 )
	{
		new->sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	}

#ifdef SUPPORT_IPv6
	if( new->proto == PROTO_UDPv6 )
	{
		new->sock = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);
	}
#endif

	if( new->sock < 0 )
	{
		fprintf(stderr, "nemozem vytvorit socket !\n");
		destroySockUdp(new);
		return NULL;
	}

	memset(&(new->sockAddr), 0, sizeof(new->sockAddr));

	if( new->proto == PROTO_UDPv4 )
	{
		new->sockAddr.sin_family = AF_INET;
		new->sockAddr.sin_port = htons(port);
		new->sockAddr.sin_addr.s_addr = inet_addr(address);
	}

#ifdef SUPPORT_IPv6
	if( new->proto == PROTO_UDPv6 )
	{
		new->sockAddr6.sin6_family = AF_INET6;
		new->sockAddr6.sin6_port = htons(port);
		inet_pton(AF_INET6, address, &(new->sockAddr6.sin6_addr));
	}
#endif
	return new;
}

int readUdpSocket(sock_udp_t *src, sock_udp_t *dst, void *address, int len)
{
	int addrlen;
	int size;

	assert( src != NULL );
	assert( dst != NULL );
	assert( address != NULL );

	if( dst->proto == PROTO_UDPv4 )
	{
	        addrlen = sizeof(src->sockAddr);

		size = recvfrom(src->sock, address, len, 0,
			(struct sockaddr *)&dst->sockAddr, (socklen_t *)&addrlen);
	}

#ifdef SUPPORT_IPv6
	if( dst->proto == PROTO_UDPv6 )
	{
 	       addrlen = sizeof(src->sockAddr6);

		size = recvfrom(src->sock, address, len, 0,
			(struct sockaddr *)&dst->sockAddr6, (socklen_t *)&addrlen);
	}
#endif

        if( size < 0 )
        {
		char str_ip[STR_IP_SIZE];

		getSockUdpIp(dst, str_ip, STR_IP_SIZE);

		fprintf(stderr, "nemozem zapisat zo socketu %d %s %d!\n", size, str_ip, getSockUdpPort(dst));
		return -1;
        }

	return size;
}

int writeUdpSocket(sock_udp_t *src, sock_udp_t *dst, void *address, int len)
{
	int addrlen;
	int size;

	assert( src != NULL );
	assert( dst != NULL );
	assert( address != NULL );

	if( dst->proto == PROTO_UDPv4 )
	{
		addrlen = sizeof(src->sockAddr);

		size = sendto(src->sock, address, len, 0,
			(struct sockaddr *)&dst->sockAddr, (socklen_t)addrlen);
	}

#ifdef SUPPORT_IPv6
	if( dst->proto == PROTO_UDPv6 )
	{
		addrlen = sizeof(src->sockAddr6);

		size = sendto(src->sock, address, len, 0,
			(struct sockaddr *)&dst->sockAddr6, (socklen_t)addrlen);
	}
#endif

       if( size < 0 )
       {
		char str_ip[STR_IP_SIZE];

		getSockUdpIp(dst, str_ip, STR_IP_SIZE);

		fprintf(stderr, "nemozem zapisat na socket %d %s %d!\n", size, str_ip, getSockUdpPort(dst));
		return -1;
       }

	return size;
}

void getSockUdpIp(sock_udp_t *p, char *str_ip, int len)
{
	assert( p != NULL  );
	assert( str_ip != NULL );

	//strcpy(str_ip, inet_ntoa(p->sockAddr.sin_addr));
	
	if( p->proto == PROTO_UDPv4 )
	{
		inet_ntop(AF_INET, &(p->sockAddr.sin_addr), str_ip, len);
	}

#ifdef SUPPORT_IPv6
	if( p->proto == PROTO_UDPv6 )
	{
		inet_ntop(AF_INET6, &(p->sockAddr6.sin6_addr), str_ip, len);
	}
#endif
}

int getSockUdpPort(sock_udp_t *p)
{
	assert( p != NULL );

	if( p->proto == PROTO_UDPv4 )
	{
		return htons(p->sockAddr.sin_port);
	}

#ifdef SUPPORT_IPv6
	if( p->proto == PROTO_UDPv6 )
	{
		return htons(p->sockAddr6.sin6_port);
	}
#endif

	assert( ! "bad proto !" );

	return -1;
}


void closeUdpSocket(sock_udp_t *p)
{
	assert( p != NULL  );

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

int main(int argc, char *argv[])
{
	sock_udp_t *sock;
	char buf[BUFSIZE];
	int ret;

	if( strcmp(argv[1], "s") == 0 )
	{
		sock_udp_t *cli;

		//sock = bindUdpSocket("::1", atoi(argv[2]), PROTO_UDPv6 );
		sock = bindUdpSocket("127.0.0.1", atoi(argv[2]), PROTO_UDPv4 );

		//cli = newSockUdp(PROTO_UDPv6);
		cli = newSockUdp(PROTO_UDPv4);
	
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
		//sock = connectUdpSocket("::1", atoi(argv[2]), PROTO_UDPv6);
		sock = connectUdpSocket("127.0.0.1", atoi(argv[2]), PROTO_UDPv4);

		strcpy(buf, "Hello world !\n");
		writeUdpSocket(sock, sock, buf, strlen(buf));

		memset(buf, 0, BUFSIZE);
		readUdpSocket(sock, sock, buf, BUFSIZE);

		printf("buf = %s", buf);
		//while(1)sleep(1);
		closeUdpSocket(sock);
	}

	return 0;
}
#endif

