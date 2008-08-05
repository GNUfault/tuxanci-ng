
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/poll.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <assert.h>
#include <netdb.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h> 
#include <fcntl.h>

#include "tcp.h"

sock_tcp_t* newSockTcp(int proto)
{
	sock_tcp_t *new;

	new = malloc( sizeof(sock_tcp_t) );
	memset(new, 0, sizeof(sock_tcp_t));
	new->proto = proto;

	return new;
}

void destroySockTcp(sock_tcp_t *p)
{
	assert( p != NULL );
	free(p);
}

sock_tcp_t* bindTcpSocket(char *address, int port, int proto)
{
	sock_tcp_t *new;
	int len;
	int ret;

	assert( port > 0 && port < 65535 );

	new = newSockTcp(proto);

	assert( new != NULL );


	if( new->proto == PROTO_TCPv4 )
	{
		new->sock = socket(AF_INET, SOCK_STREAM, 0);
	}

#ifdef SUPPORT_IPv6
	if( new->proto == PROTO_TCPv6 )
	{
		new->sock = socket(AF_INET6, SOCK_STREAM, 0);
	}
#endif	

	if( new->sock < 0 )
	{
		fprintf(stderr, "Nemozem vytvorit sietovy socket\n");
		destroySockTcp(new);
		return NULL;
	}

	if( new->proto == PROTO_TCPv4 )
	{
		new->sockAddr.sin_family = AF_INET;
		inet_pton(AF_INET, address, &(new->sockAddr.sin_addr));
		new->sockAddr.sin_port = htons(port);

		len = sizeof(new->sockAddr);
		ret = bind(new->sock, (struct sockaddr *)&new->sockAddr, len);
	}

#ifdef SUPPORT_IPv6
	if( new->proto == PROTO_TCPv6 )
	{
		new->sockAddr6.sin6_family = AF_INET6;
		//new->sockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
		inet_pton(AF_INET6, address, &(new->sockAddr6.sin6_addr));
		new->sockAddr6.sin6_port = htons(port);

		len = sizeof(new->sockAddr6);
		ret = bind(new->sock, (struct sockaddr *)&new->sockAddr6, len);
	}
#endif

	if( ret < 0 )
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

	new = newSockTcp(p->proto);

	if( new->proto == PROTO_TCPv4 )
	{
		client_len = sizeof(new->sockAddr);
		
		new->sock = accept(p->sock, (struct sockaddr *)&new->sockAddr,
				(socklen_t *)&client_len);
	}

#ifdef SUPPORT_IPv6
	if( new->proto == PROTO_TCPv6 )
	{
		client_len = sizeof(new->sockAddr6);
		
		new->sock = accept(p->sock, (struct sockaddr *)&new->sockAddr6,
				(socklen_t *)&client_len);
	}
#endif

	if( new->sock < 0 )
	{	
		//printf("XXX\n");
		destroySockTcp(new);
		return NULL;
	}

	return new;
}

void getSockTcpIp(sock_tcp_t *p, char *str_ip, int len)
{
	assert( p != NULL  );
	assert( str_ip != NULL );

	
	if( p->proto == PROTO_TCPv4 )
	{
		inet_ntop(AF_INET, &(p->sockAddr.sin_addr), str_ip, len);
		strcpy(str_ip, inet_ntoa(p->sockAddr.sin_addr));
		//printf("strcpy(str_ip, inet_ntoa(p->sockAddr.sin_addr));\n");
	}

#ifdef SUPPORT_IPv6
	if( p->proto == PROTO_TCPv6 )
	{
		inet_ntop(AF_INET6, &(p->sockAddr6.sin6_addr), str_ip, len);
	}
#endif
}

int getSockTcpPort(sock_tcp_t *p)
{
	assert( p != NULL );

	if( p->proto == PROTO_TCPv4 )
	{
		return htons(p->sockAddr.sin_port);
	}

#ifdef SUPPORT_IPv6
	if( p->proto == PROTO_TCPv6 )
	{
		return htons(p->sockAddr6.sin6_port);
	}
#endif

	assert( ! "bad proto !" );

	return -1;
}

sock_tcp_t* connectTcpSocket(char *ip, int port, int proto)
{
	sock_tcp_t *new;
	int len;
	int ret;

	assert( ip != NULL );
	assert( port > 0 && port < 65535 );

	new = newSockTcp(proto);

	if( new->proto == PROTO_TCPv4 )
	{
		new->sock = socket(AF_INET, SOCK_STREAM, 0);
	}

#ifdef SUPPORT_IPv6
	if( new->proto == PROTO_TCPv6 )
	{
		new->sock = socket(AF_INET6, SOCK_STREAM, 0);
	}
#endif	

	if( new->sock < 0 )
	{
		fprintf(stderr, "Nemozem vytvorit sietovy socket\n");
		destroySockTcp(new);
		return NULL;
	}

	if( new->proto == PROTO_TCPv4 )
	{
		new->sockAddr.sin_family = AF_INET;
		new->sockAddr.sin_addr.s_addr = inet_addr(ip);
		new->sockAddr.sin_port = htons(port);

		len = sizeof(new->sockAddr);
		ret = connect(new->sock, (struct sockaddr *)&new->sockAddr, len);
	}

#ifdef SUPPORT_IPv6
	if( new->proto == PROTO_TCPv6 )
	{
		new->sockAddr6.sin6_family = AF_INET6;
		inet_pton(AF_INET6, ip, &(new->sockAddr6.sin6_addr));
		new->sockAddr6.sin6_port = htons(port);

		len = sizeof(new->sockAddr6);
		ret = connect(new->sock, (struct sockaddr *)&new->sockAddr6, len);
	}
#endif

	if( ret < 0 )
	{
		fprintf(stderr, "Nemozem sa pripojit na %s %d\n", ip, port);
		destroySockTcp(new);
		return NULL;
	}


	return new;
}

int disableNagle(sock_tcp_t *p)
{
	int flag = 1;
	
	int result;

	result = setsockopt(p->sock, /* socket affected */
		IPPROTO_TCP,      /* set option at TCP level */
		TCP_NODELAY,      /* name of option */
		(char *) &flag,   /* the cast is historical cruft */
		sizeof(int));     /* length of option value */
	
#if 0
	if( result < 0 )
	{
		printf("disabled nagle error\n");
	}
	else
	{
		printf("disabled nagle OK\n");
	}
#endif

	return result;
}

int setTcpSockNonBlock(sock_tcp_t *p)
{
	/* Set to nonblocking socket mode */
#ifndef __WIN32__
	int oldFlag;

	oldFlag = fcntl (p->sock, F_GETFL, 0);

	if( fcntl(p->sock, F_SETFL, oldFlag | O_NONBLOCK) == -1 )
	{
		//printf("error setTcpSockNonBlock\n");
		return -1;
	}

	//printf("setTcpSockNonBlock OK\n");
#else
	unsigned long arg = 1;
	// Operation is  FIONBIO. Parameter is pointer on non-zero number.
	if( ioctlsocket(p->sock, FIONBIO, &arg) == SOCKET_ERROR )
	{
		WSACleanup();
		return -1;
	}	
#endif
	return 0;
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

#if 0
#define MAX_CLIENTS	32
#define BUF_SIZE	512

int main(int argc, char **argv)
{
	sock_tcp_t *sock;
	sock_tcp_t *client[MAX_CLIENTS];
	int count_client;
	int changed, i;
	struct pollfd *tmp;

	sock = bindTcpSocket( "127.0.0.1", 2200, PROTO_TCPv4 );
	disableNagle(sock);
	count_client = 0;

	for(;;)
	{
		tmp = malloc( (count_client+1) * sizeof(struct pollfd) );
	
		tmp[0].fd = sock->sock;
		tmp[0].events = POLLIN;
		tmp[0].revents = 0;

		for( i = 0 ; i < count_client ; i++ )
		{
			tmp[1+i].fd = client[i]->sock;
			tmp[1+i].events = POLLIN;
			tmp[1+i].revents = 0;
		}
	
		changed = poll(tmp, count_client+1, 1000);
	
		if( tmp[0].revents & POLLIN != 0 )
		{
			client[count_client] = getTcpNewClient(sock);
			disableNagle(client[count_client]);
			count_client++;
		}
	
		for( i = 0 ; i < count_client ; i++ )
		{
			if( tmp[1+i].revents & POLLIN != 0 )
			{
				char buf[BUF_SIZE];
				int ret;
	
				ret = read(client[i]->sock, buf, BUF_SIZE);

				if( ret <= 0 )
				{
					closeTcpSocket(client[i]);

					memmove(client + i,
						client + (i+1),
						((count_client-1) - i) * sizeof(sock_tcp_t *) );

					count_client--;
					continue;
				}

				write(client[i]->sock, buf, ret);
			}
		}

		free(tmp);
	}
}
#endif

