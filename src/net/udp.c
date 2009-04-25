#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef __WIN32__
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#else
#include <windows.h>
#include <wininet.h>
#endif

#include <unistd.h>
#include <assert.h>
#include <fcntl.h>

#include "main.h"
#include "net_multiplayer.h"

#include "udp.h"
#include "dns.h"

#define BUFSIZE 1000

static int getProto(char *str)
{
	if (strstr(str, ".") != NULL)
		return PROTO_UDPv4;
	if (strstr(str, ":") != NULL)
		return PROTO_UDPv6;

	assert(!_("[Error] Network protocol not detected"));
	return -1;
}

sock_udp_t *sock_udp_new(void)
{
	sock_udp_t *new;

	new = malloc(sizeof(sock_udp_t));
	memset(new, 0, sizeof(sock_udp_t));

	return new;
}

void sock_udp_destroy(sock_udp_t *p)
{
	assert(p != NULL);
	free(p);
}

sock_udp_t *sock_udp_bind(char *address, int port)
{
	sock_udp_t *new;
	ret = -1;	/* no warnings */

	assert(port > 0 && port < 65535);

	new = sock_udp_new();
	new->proto = getProto(address);

	assert(new != NULL);

	if (new->proto == PROTO_UDPv4) {
		new->sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	}
#ifdef SUPPORT_IPv6
	if (new->proto == PROTO_UDPv6) {
		new->sock = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);
	}
#endif

	if (new->sock < 0) {
		fprintf(stderr, _("[Error] Unable to create UDP socket\n"));
		sock_udp_destroy(new);
#ifdef __WIN32__
		WSACleanup();
#endif
		return NULL;
	}

	memset(&(new->sockAddr), 0, sizeof(new->sockAddr));

	if (new->proto == PROTO_UDPv4) {
		new->sockAddr.sin_family = AF_INET;
		new->sockAddr.sin_port = htons(port);
		new->sockAddr.sin_addr.s_addr = inet_addr(address);

		res = bind(new->sock, (struct sockaddr *) &(new->sockAddr), sizeof(new->sockAddr));
	}
#ifdef SUPPORT_IPv6
	if (new->proto == PROTO_UDPv6) {
		new->sockAddr6.sin6_family = AF_INET6;
		new->sockAddr6.sin6_port = htons(port);
		inet_pton(AF_INET6, address, &(new->sockAddr6.sin6_addr));

		res = bind(new->sock, (struct sockaddr *) &(new->sockAddr6), sizeof(new->sockAddr6));
	}
#endif

	if (res < 0) {
		fprintf(stderr, _("[Error] Unable to bind UDP socket to [%s]:%d\n"), address, port);
		sock_udp_destroy(new);
#ifdef __WIN32__
		WSACleanup();
#endif
		return NULL;
	}

	return new;
}

sock_udp_t *sock_udp_connect(char *address, int port)
{
	sock_udp_t *new;

	assert(address != NULL);
	assert(port > 0 && port < 65536);

	new = sock_udp_new();
	new->proto = getProto(address);

	assert(new != NULL);

	if (new->proto == PROTO_UDPv4) {
		new->sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	}
#ifdef SUPPORT_IPv6
	if (new->proto == PROTO_UDPv6) {
		new->sock = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);
	}
#endif

	if (new->sock < 0) {
		fprintf(stderr, _("[Error] Unable to create UDP socket for connecting\n"));
		sock_udp_destroy(new);
#ifdef __WIN32__
		WSACleanup();
#endif
		return NULL;
	}

	char *domain = dns_resolv(address);

	if (domain) {
		address = domain;
	}

	memset(&(new->sockAddr), 0, sizeof(new->sockAddr));

	if (new->proto == PROTO_UDPv4) {
		new->sockAddr.sin_family = AF_INET;
		new->sockAddr.sin_port = htons(port);
		new->sockAddr.sin_addr.s_addr = inet_addr(address);
	}
#ifdef SUPPORT_IPv6
	if (new->proto == PROTO_UDPv6) {
		new->sockAddr6.sin6_family = AF_INET6;
		new->sockAddr6.sin6_port = htons(port);
		inet_pton(AF_INET6, address, &(new->sockAddr6.sin6_addr));
	}
#endif
	if (domain) {
		free(domain);
	}

	return new;
}

int sock_udp_set_non_block(sock_udp_t *p)
{
	/* set to nonblocking socket mode */
#ifndef __WIN32__
	int oldFlag;

	oldFlag = fcntl(p->sock, F_GETFL, 0);

	if (fcntl(p->sock, F_SETFL, oldFlag | O_NONBLOCK) == -1) {
		return -1;
	}
#else
	unsigned long arg = 1;
	/* operation is FIONBIO; parameter is a pointer on non-zero number */
	if (ioctlsocket(p->sock, FIONBIO, &arg) == SOCKET_ERROR) {
		WSACleanup();
		return -1;
	}
#endif
	return 0;
}

int sock_udp_read(sock_udp_t *src, sock_udp_t *dst, void *address, int len)
{
	int addrlen;
	int size = -1;	/* no warnings */

	assert(src != NULL);
	assert(dst != NULL);
	assert(address != NULL);

	if (src->proto == PROTO_UDPv4) {
		addrlen = sizeof(src->sockAddr);

#ifndef __WIN32
		size = recvfrom(src->sock, address, len, 0, (struct sockaddr *) &dst->sockAddr, (socklen_t *) &addrlen);
#else
		size = recvfrom(src->sock, address, len, 0, (struct sockaddr *) &dst->sockAddr, (int *) &addrlen);
#endif
	}
#ifdef SUPPORT_IPv6
	if (src->proto == PROTO_UDPv6) {
		addrlen = sizeof(src->sockAddr6);

		size = recvfrom(src->sock, address, len, 0, (struct sockaddr *) &dst->sockAddr6, (socklen_t *) &addrlen);
	}
#endif

	dst->sock = src->sock;
	dst->proto = src->proto;

	if (size < 0) {
		char str_ip[STR_IP_SIZE];

		sock_udp_get_ip(dst, str_ip, STR_IP_SIZE);

		fprintf(stderr, _("[Error] Unable to read from UDP socket [%s]:%d (read %d bytes)\n"), str_ip, sock_udp_get_port(dst), size);

#ifdef __WIN32__
		WSACleanup();
#endif
		return -1;
	}

	return size;
}

int sock_udp_write(sock_udp_t *src, sock_udp_t *dst, void *address, int len)
{
	int addrlen;
	int size = -1;	/* no warnings */

	assert(src != NULL);
	assert(dst != NULL);
	assert(address != NULL);

	if (src->proto == PROTO_UDPv4) {
		addrlen = sizeof(src->sockAddr);

		size = sendto(src->sock, address, len, 0, (struct sockaddr *) &dst->sockAddr, addrlen);
	}
#ifdef SUPPORT_IPv6
	if (src->proto == PROTO_UDPv6) {
		addrlen = sizeof(src->sockAddr6);

		size = sendto(src->sock, address, len, 0, (struct sockaddr *) &dst->sockAddr6, addrlen);
	}
#endif

	if (size < 0) {
		char str_ip[STR_IP_SIZE];

		sock_udp_get_ip(dst, str_ip, STR_IP_SIZE);

		fprintf(stderr, _("[Error] Unable to write to socket [%s]:%d with protocol %d (written %d bytes)\n"), str_ip, sock_udp_get_port(dst), dst->proto, size);

#ifdef __WIN32__
		WSACleanup();
#endif
		return -1;
	}

	return size;
}

void sock_udp_get_ip(sock_udp_t *p, char *str_ip, int len)
{
	assert(p != NULL);
	assert(str_ip != NULL);


	if (p->proto == PROTO_UDPv4) {
#ifndef __WIN32__
		inet_ntop(AF_INET, &(p->sockAddr.sin_addr), str_ip, len);
#else
		strcpy(str_ip, inet_ntoa(p->sockAddr.sin_addr));
#endif
	}
#ifdef SUPPORT_IPv6
	if (p->proto == PROTO_UDPv6) {
		inet_ntop(AF_INET6, &(p->sockAddr6.sin6_addr), str_ip, len);
	}
#endif
}

int sock_udp_get_port(sock_udp_t *p)
{
	assert(p != NULL);

	if (p->proto == PROTO_UDPv4) {
		return htons(p->sockAddr.sin_port);
	}
#ifdef SUPPORT_IPv6
	if (p->proto == PROTO_UDPv6) {
		return htons(p->sockAddr6.sin6_port);
	}
#endif

	assert(!_("[Error] Bad IP protocol"));

	return -1;
}

void sock_udp_close(sock_udp_t *p)
{
	assert(p != NULL);

#ifndef __WIN32__
	close(p->sock);
#else
	closesocket(p->sock);
	/*WSACleanup();  -- when the socket is closed correctly, it is not needed to do a cleanup */
#endif
	sock_udp_destroy(p);
}
