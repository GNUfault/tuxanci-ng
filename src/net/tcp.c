
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef __WIN32__
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <fcntl.h>
#else
#include <windows.h>
#include <wininet.h>
#endif // __WIN32__
#include <unistd.h>
#include <assert.h>

#include "tcp.h"

sock_tcp_t *sock_tcp_new(void)
{
	sock_tcp_t *new;

	new = malloc(sizeof(sock_tcp_t));
	memset(new, 0, sizeof(sock_tcp_t));

	return new;
}

void sock_tcp_destroy(sock_tcp_t * p)
{
	assert(p != NULL);
	free(p);
}

static int getProto(char *str)
{
	if( strstr(str, ".") != NULL  )return PROTO_TCPv4;
	if( strstr(str, ":") != NULL  )return PROTO_TCPv6;

	assert( ! "Protocol not detected !" );
	return -1;
}

sock_tcp_t *sock_tcp_bind(char *address, int port)
{
	sock_tcp_t *new;
	unsigned long param_setsock = 1;
	int len;
	int ret;

	assert(port > 0 && port < 65535);

	new = sock_tcp_new();
	new->proto = getProto(address);
	ret = -1;					// no Warnings

	assert(new != NULL);

	if (new->proto == PROTO_TCPv4) {
		new->sock = socket(AF_INET, SOCK_STREAM, 0);
	}

#ifdef SUPPORT_IPv6
	if (new->proto == PROTO_TCPv6) {
		new->sock = socket(AF_INET6, SOCK_STREAM, 0);
	}
#endif

	if (new->sock < 0) {
		fprintf(stderr, _("Unable to create socket when connecting!\n"));
		sock_tcp_destroy(new);
#ifdef __WIN32__
		WSACleanup();
#endif
		return NULL;
	}

	setsockopt(new->sock, SOL_SOCKET, SO_REUSEADDR, (char *) &param_setsock,
			   sizeof(param_setsock));

	if (new->proto == PROTO_TCPv4) {
		new->sockAddr.sin_family = AF_INET;
#ifndef __WIN32__
		inet_pton(AF_INET, address, &(new->sockAddr.sin_addr));
#else
		new->sockAddr.sin_addr.s_addr = inet_addr(address);
#endif
		new->sockAddr.sin_port = htons(port);

		len = sizeof(new->sockAddr);
		ret = bind(new->sock, (struct sockaddr *) &new->sockAddr, len);
	}
#ifdef SUPPORT_IPv6
	if (new->proto == PROTO_TCPv6) {
		new->sockAddr6.sin6_family = AF_INET6;
		//new->sockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
#ifndef __WIN32__
		inet_pton(AF_INET6, address, &(new->sockAddr6.sin6_addr));
#else
		new->sockAddr6.sin6_addr.s_addr = inet_addr(address);
#endif
		new->sockAddr6.sin6_port = htons(port);

		len = sizeof(new->sockAddr6);
		ret = bind(new->sock, (struct sockaddr *) &new->sockAddr6, len);
	}
#endif

	if (ret < 0) {
		fprintf(stderr, _("Unable to bint to port: %d\n"), port);
		sock_tcp_destroy(new);
#ifdef __WIN32__
		WSACleanup();
#endif
		return NULL;
	}

	listen(new->sock, 5);

	return new;
}

sock_tcp_t *sock_tcp_accept(sock_tcp_t * p)
{
	sock_tcp_t *new;
	int client_len;

	assert(p != NULL);
	assert(p->sock >= 0);

	new = sock_tcp_new();
	new->proto = p->proto;

	if (new->proto == PROTO_TCPv4) {
		client_len = sizeof(new->sockAddr);
#ifndef __WIN32__
		new->sock = accept(p->sock, (struct sockaddr *) &new->sockAddr, (socklen_t *) & client_len);
#else
		new->sock = accept(p->sock, (struct sockaddr *) &new->sockAddr, (long *) & client_len);
#endif
	}
#ifdef SUPPORT_IPv6
	if (new->proto == PROTO_TCPv6) {
		client_len = sizeof(new->sockAddr6);
#ifndef __WIN32__
		new->sock = accept(p->sock, (struct sockaddr *) &new->sockAddr6, (socklen_t *) & client_len);
#else
		new->sock = accept(p->sock, (struct sockaddr *) &new->sockAddr6, (long *) & client_len);
#endif
	}
#endif

	if (new->sock < 0) {
		//printf("XXX\n");
		sock_tcp_destroy(new);
#ifdef __WIN32__
		WSACleanup();
#endif
		return NULL;
	}

	return new;
}

void sock_tcp_get_ip(sock_tcp_t * p, char *str_ip, int len)
{
	assert(p != NULL);
	assert(str_ip != NULL);


	if (p->proto == PROTO_TCPv4) {
#ifndef __WIN32__
		inet_ntop(AF_INET, &(p->sockAddr.sin_addr), str_ip, len);
#else
		strcpy(str_ip, inet_ntoa(p->sockAddr.sin_addr));
#endif
		//printf("strcpy(str_ip, inet_ntoa(p->sockAddr.sin_addr));\n");
	}
#ifdef SUPPORT_IPv6
	if (p->proto == PROTO_TCPv6) {
#ifndef __WIN32__
		inet_ntop(AF_INET6, &(p->sockAddr6.sin6_addr), str_ip, len);
#else
		// not tested
		strcpy(str_ip, inet_ntoa(p->sockAddr6.sin6_addr));
#endif // __WIN32__
	}
#endif // SUPPORT_IPv6
}

int sock_tcp_get_port(sock_tcp_t * p)
{
	assert(p != NULL);

	if (p->proto == PROTO_TCPv4) {
		return htons(p->sockAddr.sin_port);
	}
#ifdef SUPPORT_IPv6
	if (p->proto == PROTO_TCPv6) {
		return htons(p->sockAddr6.sin6_port);
	}
#endif

	assert(!_("Bad IP proto!"));

	return -1;
}

sock_tcp_t *sock_tcp_connect(char *ip, int port)
{
	sock_tcp_t *new;
	int len;
	int ret;

	assert(ip != NULL);
	assert(port > 0 && port < 65535);

	new = sock_tcp_new();
	new->proto = getProto(ip);
	ret = -1;					// no Warnnings

	if (new->proto == PROTO_TCPv4) {
		new->sock = socket(AF_INET, SOCK_STREAM, 0);
	}

#ifdef SUPPORT_IPv6
	if (new->proto == PROTO_TCPv6) {
		new->sock = socket(AF_INET6, SOCK_STREAM, 0);
	}
#endif

	if (new->sock < 0) {
		fprintf(stderr, _("Unable to create TCP socket!\n"));
		sock_tcp_destroy(new);
#ifdef __WIN32__
		WSACleanup();
#endif
		return NULL;
	}

	if (new->proto == PROTO_TCPv4) {
		new->sockAddr.sin_family = AF_INET;
		new->sockAddr.sin_addr.s_addr = inet_addr(ip);
		new->sockAddr.sin_port = htons(port);

		len = sizeof(new->sockAddr);
		ret = connect(new->sock, (struct sockaddr *) &new->sockAddr, len);
	}

#ifdef SUPPORT_IPv6
	if (new->proto == PROTO_TCPv6) {
		new->sockAddr6.sin6_family = AF_INET6;
		inet_pton(AF_INET6, ip, &(new->sockAddr6.sin6_addr));
		new->sockAddr6.sin6_port = htons(port);

		len = sizeof(new->sockAddr6);
		ret = connect(new->sock, (struct sockaddr *) &new->sockAddr6, len);
	}
#endif

	if (ret < 0) {
		fprintf(stderr, "Unable to connect on: \"%s\" port: \"%d\"\n", ip,
				port);
		sock_tcp_destroy(new);
#ifdef __WIN32__
		WSACleanup();
#endif
		return NULL;
	}


	return new;
}

int sock_tcp_diable_nagle(sock_tcp_t * p)
{
	int flag = 1;

	int result;

	result = setsockopt(p->sock,	/* socket affected */
			   IPPROTO_TCP,	/* set option at TCP level */
			   TCP_NODELAY,	/* name of option */
			   (char *) &flag,	/* the cast is historical cruft */
			   sizeof(int));	/* length of option value */

#if 0
	if (result < 0) {
		printf("disabled nagle error\n");
	} else {
		printf("disabled nagle OK\n");
	}
#endif

	return result;
}

int sock_tcp_set_non_block(sock_tcp_t * p)
{
	/* Set to nonblocking socket mode */
#ifndef __WIN32__
	int oldFlag;

	oldFlag = fcntl(p->sock, F_GETFL, 0);

	if (fcntl(p->sock, F_SETFL, oldFlag | O_NONBLOCK) == -1) {
		//printf("error sock_tcp_set_non_block\n");
		return -1;
	}
	//printf("sock_tcp_set_non_block OK\n");
#else
	unsigned long arg = 1;
	// Operation is  FIONBIO. Parameter is pointer on non-zero number.
	if (ioctlsocket(p->sock, FIONBIO, &arg) == SOCKET_ERROR) {
		WSACleanup();
		return -1;
	}
#endif
	return 0;
}

int sock_tcp_read(sock_tcp_t * p, void *address, int len)
{
	assert(p != NULL);
	assert(address != NULL);

	return read(p->sock, address, len);
}

int sock_tcp_write(sock_tcp_t * p, void *address, int len)
{
	assert(p != NULL);
	assert(address != NULL);

	return write(p->sock, address, len);
}

void sock_tcp_close(sock_tcp_t * p)
{
	assert(p != NULL);

	close(p->sock);
	sock_tcp_destroy(p);
#ifdef __WIN32__
	WSACleanup();
#endif
}
