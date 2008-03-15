
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <assert.h>

#include "tcp.h"

#define BUFSIZE 1000

sock_tcp_t* newSockTcp()
{
	sock_tcp_t *new;

	new = malloc( sizeof(sock_tcp_t) );
	memset(new, 0, sizeof(sock_tcp_t));

	return new;
}

void destroySockTcp(sock_tcp_t *p)
{
	assert( p != NULL );
	free(p);
}

sock_tcp_t* bindTcpSocket(int port)
{
	sock_tcp_t *new;
	int len;

	assert( port > 0 && port < 65535 );

	new = newSockTcp();

	assert( new != NULL );

	new->sock = socket(AF_INET, SOCK_STREAM, 0);

	if( new->sock < 0 )
	{
		fprintf(stderr, "Nemozem vytvorit sietovy socket\n");
		destroySockTcp(new);
		return NULL;
	}

	new->sockAddr.sin_family = AF_INET;
	new->sockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	new->sockAddr.sin_port = htons(port);
	
	len = sizeof(new->sockAddr);

	if ( bind(new->sock, (struct sockaddr *)&new->sockAddr, len) < 0 )
	{
		fprintf(stderr, "Nemozem obsadit sietovy port %d\n", port);
		destroySockTcp(new);
		return NULL;
	}

	listen(new->sock, 5);

	return new;
}

sock_tcp_t* getTcpNewClient(sock_tcp_t *p)
{
	sock_tcp_t *new;
	int client_len;

	assert( p != NULL );
	assert( p->sock >= 0 );

	new = newSockTcp();

	client_len = sizeof(new->sockAddr);
	
	new->sock = accept(p->sock, (struct sockaddr *)&new->sockAddr,
			       (socklen_t *)&client_len);

	return new;
}

void getSockTcpIp(sock_tcp_t *p, char *str_ip)
{
	assert( p != NULL );
	assert( str_ip != NULL );

	strcpy(str_ip, inet_ntoa(p->sockAddr.sin_addr));
}

int getSockTcpPort(sock_tcp_t *p)
{
	assert( p != NULL );
	return htons(p->sockAddr.sin_port);
}

sock_tcp_t* connectTcpSocket(char *ip, int port)
{
	sock_tcp_t *new;
	int len;

	assert( ip != NULL );
	assert( port > 0 && port < 65535 );

	new = newSockTcp();

	new->sock = socket(AF_INET, SOCK_STREAM, 0);

	if( new->sock < 0 )
	{
		fprintf(stderr, "Nemozem vytvorit sietovy socket\n");
		destroySockTcp(new);
		return NULL;
	}

	new->sockAddr.sin_family = AF_INET;
	new->sockAddr.sin_addr.s_addr = inet_addr(ip);
	new->sockAddr.sin_port = htons(port);
	
	len = sizeof(new->sockAddr);

	if ( connect(new->sock, (struct sockaddr *)&new->sockAddr, len) < 0 )
	{
		fprintf(stderr, "Nemozem sa pripojit na %s %d\n", ip, port);
		destroySockTcp(new);
		return NULL;
	}

	return new;
}

int readTcpSocket(sock_tcp_t *p, void *address, int len)
{
	assert( p != NULL );
	assert( address != NULL );

	return read(p->sock, address, len);
}

int writeTcpSocket(sock_tcp_t *p, void *address, int len)
{
	assert( p != NULL );
	assert( address != NULL );

	return write(p->sock, address, len);
}

void closeTcpSocket(sock_tcp_t *p)
{
	assert( p != NULL );

	close(p->sock);
	destroySockTcp(p);
}

static int myWait(sock_tcp_t *p)
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

int test_tcp_main(int argc, char **argv)
{
	sock_tcp_t *sock;
	char buf[BUFSIZE];
	int ret;

	if( strcmp(argv[1], "s") == 0 )
	{
		sock_tcp_t *cli;

		sock = bindTcpSocket( atoi(argv[2]) );
	
		while(1)
		{
			while( myWait(sock) == 0 )
				;

			cli = getTcpNewClient(sock);

			memset(buf, 0, BUFSIZE);
			ret = readTcpSocket(cli, buf, BUFSIZE);
			writeTcpSocket(cli, buf, ret);

			closeTcpSocket(cli);
		}
	}

	if( strcmp(argv[1], "c") == 0 )
	{
		sock = connectTcpSocket("127.0.0.1", atoi(argv[2]));
	
		strcpy(buf, "Hello world !\n");
		writeTcpSocket(sock, buf, strlen(buf));

		memset(buf, 0, BUFSIZE);
		readTcpSocket(sock, buf, BUFSIZE);

		printf("buf = %s", buf);
		
		closeTcpSocket(sock);
	}

	return 0;
}
