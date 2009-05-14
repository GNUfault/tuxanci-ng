/*
 *  (C) Copyright 2008 ZeXx86 (zexx86@gmail.com)
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "sockets.h"
#include "resolve.h"
#include "defaults.h"


typedef unsigned net_ipv4;

#define NET_IPV4_TO_ADDR(a, b, c, d) \
	(((net_ipv4)(a) << 24) | (((net_ipv4)(b) & 0xff) << 16) | (((net_ipv4)(c) & 0xff) << 8) | ((net_ipv4)(d) & 0xff))


server_t server_list; // structure of all servers 
client_t client_list; // structure with all connected clients

extern struct sockaddr_in clientInfo;

/* new client is connected, we need create new structure  */
unsigned client_connected (unsigned int client)
{
	if (client > 0) {
		// For communication is used socket "client"
		client_t *ctx;
		
		// alloc and init context
		ctx = (client_t*) malloc (sizeof (client_t));

		ctx->socket = client;
		ctx->ip = clientInfo.sin_addr.s_addr;

		// add into list  
		ctx->next = &client_list;
		ctx->prev = client_list.prev;
		ctx->prev->next = ctx;
		ctx->next->prev = ctx;
	}
	
	return 1;
}

unsigned client_disconnected (client_t *client)
{
	unsigned id = client->socket;

	printf ("> Client -> disconnected: %d\n", id);

 	client->next->prev = client->prev;
 	client->prev->next = client->next; 

//	free (client);			// FIXME memleak

	close (id);

	return 1;
}


/* register new server  */
unsigned server_register (client_t *client, char *buf, unsigned int size)
{
	response_head *header = (response_head *) buf;

	server_t *server;
	for (server = server_list.next; server != &server_list; server = server->next) {
		if (server->ip == client->ip && server->port == header->port) {
			client_disconnected (client);
			return 0;
		}
	}

	server_t *ctx;
		
	// alloc and init context
	ctx = (server_t *) malloc (sizeof (server_t));

	ctx->state = SERVER_STATE_ONLINE;
	ctx->socket = client->socket;

	ctx->ip = client->ip;
	ctx->port = header->port;

	// add into list
	ctx->next = &server_list;
	ctx->prev = server_list.prev;
	ctx->prev->next = ctx;
	ctx->next->prev = ctx;

	struct sockaddr_in srv;
	srv.sin_addr.s_addr = ctx->ip;

	printf ("> New server %s:%d registered\n", inet_ntoa (srv.sin_addr), ctx->port);
	
	return 1;
}

/* register new server  */
unsigned server_getlist (client_t *client, char *buf, unsigned int size)
{

	unsigned len = 0;

	server_t *server;
	for (server = server_list.next; server != &server_list; server = server->next)
		len += 9;

	char *str = (char *) malloc (sizeof (char) * (len + 1));

	if (!str)
		return 0;

	response_head *header = (response_head *) malloc (sizeof (response_head));

	if (!header)
		return 0;

	unsigned i = 0;
	for (server = server_list.next; server != &server_list; server = server->next) {
		header->port = server->port;
		header->ip = server->ip;

		memcpy (str+i, (void *) header, 8);

		str[i + 8] = '\n';

		i += 9;
	}

	send_to_socket (client->socket, str, i);

	free (header);

	return 1;
}

unsigned server_add (unsigned ip, unsigned port)
{
	server_t *ctx;
		
	// alloc and init context
	ctx = (server_t *) malloc (sizeof (server_t));

	ctx->state = SERVER_STATE_ONLINE;
	ctx->socket = 0;

	ctx->ip = htonl (ip);
	ctx->port = port;

	// add into list
	ctx->next = &server_list;
	ctx->prev = server_list.prev;
	ctx->prev->next = ctx;
	ctx->next->prev = ctx;

	return 1;
}

void *thread_server_update (void *unused)
{
	struct sockaddr_in srv;

	sleep (1);

	while (1) {
		server_t *server;
		for (server = server_list.next; server != &server_list; server = server->next) {
			srv.sin_addr.s_addr = server->ip;
			printf ("> Server %s refresh\n", inet_ntoa (srv.sin_addr));
			int ret = server_resolve (server->ip, server->port);

			if (ret == -1) {
				if (server->state == SERVER_STATE_TIMEOUT)
					goto out;

				if (server->state == SERVER_STATE_LAG) {
					server->state = SERVER_STATE_TIMEOUT;
					break;
				}

				server->state = SERVER_STATE_LAG;
				continue;
			}      

			if (!ret) { // vymazeme server ze seznamu
out:
				if (server->state == SERVER_STATE_TIMEOUT)
					printf ("> Server %s is timeout\n", inet_ntoa (srv.sin_addr));
				else
					printf ("> Server %s is offline\n", inet_ntoa (srv.sin_addr));

				server->next->prev = server->prev;
 				server->prev->next = server->next; 

				server->state = SERVER_STATE_OFFLINE;

				free (server);
				break;
			}
		}

		sleep (DEFAULT_REFRESH_INTERVAL);
	}

	return (NULL);
}

/* handler of all received data */
unsigned handler (client_t *client, char *buf, int size)
{
	if (size < 1)
		return 0;

	//printf ("buf: %s\n", buf);

	switch (buf[0]) {
		case 'l':
			server_getlist (client, buf, size);
			client_disconnected (client);
			break;
		case 'p':
			if (size == 9)
				server_register (client, buf+1, size-1);
			break;
	
	}

	return 1;
}


unsigned init_resolve ()
{
	server_list.next = &server_list;
	server_list.prev = &server_list;

	client_list.next = &client_list;
	client_list.prev = &client_list;

	// set default ctx values
	server_list.socket = 0;
	server_list.state = SERVER_STATE_ONLINE;

	/** PRESET server */
	//server_add (NET_IPV4_TO_ADDR (87, 236, 195, 16), 6800);

	pthread_t thread;
	pthread_create(&thread, NULL, &thread_server_update, NULL);

	return 1;
}
