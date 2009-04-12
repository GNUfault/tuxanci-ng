
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "main.h"
#include "list.h"
#include "textFile.h"
#include "homeDirector.h"
#include "net_multiplayer.h"

#include "interface.h"
#include "screen.h"
#include "image.h"
#include "hotKey.h"

#ifndef __WIN32__
#    include <sys/socket.h>
#    include <sys/select.h>
#    include <sys/types.h>
#    include <netinet/in.h>
#    include <arpa/inet.h>
#    include <netdb.h>
#else
#    include <windows.h>
#    include <wininet.h>
#endif

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include "udp.h"
#include "dns.h"

extern int errno;

#ifndef NO_SOUND
#    include "music.h"
#endif

#include "gameType.h"
#include "browser.h"

#include "widget.h"
#include "widget_image.h"
#include "widget_label.h"
#include "widget_button.h"
#include "widget_select.h"

#define SERVER_TIMEOUT	200

static widget_t *image_backgorund;

static widget_t *button_play;
static widget_t *button_back;
static widget_t *button_refresh;

static widget_t *label_server;
static widget_t *label_version;
static widget_t *label_address;
static widget_t *label_arena;
static widget_t *label_players;
static widget_t *label_ping;

static widget_t *select_server;

static server_t server_list;

/* function prototypes */
static int LoadServers();
static int RefreshServers();
static server_t *server_getcurr();

static void hotkey_escape()
{
	screen_set("gameType");
}

void browser_start()
{
#ifndef NO_SOUND
	music_play("menu", MUSIC_GROUP_BASE);
#endif

	hot_key_register(SDLK_ESCAPE, hotkey_escape);

	LoadServers();
}

void browser_draw()
{
	wid_image_draw(image_backgorund);

	label_draw(label_server);
	label_draw(label_version);
	label_draw(label_address);
	label_draw(label_arena);
	label_draw(label_players);
	label_draw(label_ping);

	select_draw(select_server);

	button_draw(button_play);
	button_draw(button_back);
	button_draw(button_refresh);
}

void browser_event()
{
	select_event(select_server);

	button_event(button_play);
	button_event(button_back);
	button_event(button_refresh);
}

void browser_stop()
{
	unsigned i = 0;
	server_t *server;

	//list_destroy_item(select_server->list, free);
	//select_server->list = list_new();
	select_remove_all(select_server);

	hot_key_unregister(SDLK_ESCAPE);

	while (1) {
		i = 0;
		for (server = server_list.next; server != &server_list;
			 server = server->next) {
			server->next->prev = server->prev;
			server->prev->next = server->next;

			if (server->name)
				free(server->name);

			if (server->version)
				free(server->version);

			if (server->arena)
				free(server->arena);

			free(server);

			i++;
			break;
		}

		if (!i)
			return;
	}
}

static void eventWidget(void *p)
{
	widget_t *button;

	button = (widget_t *) (p);

	if (button == button_play) {
		server_t *server = server_getcurr();

		if (!server)
			return;

		/* server je offline */
		if (!server->state)
			return;

		struct in_addr srv;
		srv.s_addr = server->ip;

		char *address = (char *) inet_ntoa(srv);

		setSettingGameType(NET_GAME_TYPE_CLIENT);
		game_type_set_ip(address);
		game_type_set_port(server->port);

		screen_set("world");
	}

	if (button == button_back) {
		screen_set("gameType");
	}

	if (button == button_refresh) {
		RefreshServers();
	}
}

server_t *server_getcurr()
{
	int i = 0;
	server_t *server;

	for (server = server_list.next; server != &server_list;
		 server = server->next) {
		if (i == select_get_index(select_server))
			return server;

		i++;
	}

	return 0;
}

int server_getinfo(server_t * server)
{
	struct sockaddr_in srv;
	int mySocket;

	if ((mySocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
		return -1;
	}
#ifndef __WIN32__
	// Lets make socket non-blocking
	int oldFlag = fcntl(mySocket, F_GETFL, 0);
	if (fcntl(mySocket, F_SETFL, oldFlag | O_NONBLOCK) == -1) {
		return -1;
	}
#else
	unsigned long arg = 1;
	// Operation is  FIONBIO. Parameter is pointer on non-zero number.
	if (ioctlsocket(mySocket, FIONBIO, &arg) == SOCKET_ERROR) {
		WSACleanup();
		return -1;
	}
#endif

	struct timeval tv;

	fd_set myset;
	FD_ZERO(&myset);
	FD_SET(mySocket, &myset);

	tv.tv_sec = 2;
	tv.tv_usec = 0;

	srv.sin_family = AF_INET;
	srv.sin_port = htons(server->port);
	srv.sin_addr.s_addr = server->ip;

	if (connect(mySocket, (struct sockaddr *) &srv, sizeof(srv)) == -1) {
#ifndef __WIN32__
		if (errno != EINPROGRESS)
			return -1;
#else
		if (WSAGetLastError() != WSAEWOULDBLOCK) {
			WSACleanup();
			return -1;
		}
#endif
	}

	int ret = select(mySocket + 1, NULL, &myset, NULL, &tv);

	if (ret == -1)
		return 0;

	if (ret == 0)
		return -2;

	char request[8];
	memcpy(request, "status\n", 7);

	int r = send(mySocket, request, 7, 0);

	if (r == -1)
		return -2;

	FD_ZERO(&myset);
	FD_SET(mySocket, &myset);
	tv.tv_sec = 1;
	tv.tv_usec = 0;

	char *str = (char *) malloc(sizeof(char) * 257);

	if (!str)
		return -1;

	while (1) {
		int sel = select(mySocket + 1, &myset, NULL, NULL, &tv);

		if (sel == 0) {
			free(str);
#ifndef __WIN32__
			close(mySocket);
#else
			closesocket(mySocket);
			WSACleanup();
#endif
			return -2;
		}

		if (sel == -1) {
			free(str);
#ifndef __WIN32__
			close(mySocket);
#else
			closesocket(mySocket);
			WSACleanup();
#endif
			return -2;
		}

		if ((ret = recv(mySocket, str, 256, 0)) == -1) {
			free(str);
#ifndef __WIN32__
			close(mySocket);
#else
			closesocket(mySocket);
			WSACleanup();
#endif
			return -1;
		}

		if (ret > 0)
			break;
	}

#ifndef __WIN32__
	close(mySocket);
#else
	closesocket(mySocket);
	WSACleanup();
#endif

	server->ping = 1000 - (tv.tv_usec / 1000);
	/*
	   ["name: F\n"] >= svn82
	   "version: svnX\n"
	   "clients: N\n"
	   "maxclients: M\n"
	   "uptime: D\n"
	 */

	//unsigned i = 0;
	int i = 0;

	while (i < ret) {
		if (str[i] == '\n')
			str[i] = '\0';

		i++;
	}

	unsigned name = 0;

	if (strstr(str, "name: "))
		name = 1;

	int len = 0;

	if (name) {
		len = strlen(str);
		if (!strncmp(str, "name: ", 6)) {
			unsigned name_len = strlen(str + 6);
			server->name = (char *) malloc(sizeof(char) * (name_len + 2));

			if (!server->name)
				return 0;

			memcpy(server->name, str + 6, name_len + 1);
			server->name[name_len + 1] = '\0';
		}
	}

	/* HACK */
	if (!name)
		len = -1;

	if (!strncmp(str + len + 1, "version: ", 9)) {
		unsigned ver_len = strlen(str + len + 10);
		server->version = (char *) malloc(sizeof(char) * (ver_len + 2));

		if (!server->version)
			return 0;

		memcpy(server->version, str + len + 10, ver_len + 1);
		server->version[ver_len + 1] = '\0';
	}

	len += strlen(str + len + 1) + 1;


	if (!strncmp(str + len + 1, "clients: ", 9))
		server->clients = atoi(str + len + 10);

	len += strlen(str + len + 1) + 1;


	if (!strncmp(str + len + 1, "maxclients: ", 12))
		server->maxclients = atoi(str + len + 13);

	len += strlen(str + len + 1) + 1;


	if (!strncmp(str + len + 1, "uptime: ", 8))
		server->uptime = atoi(str + len + 9);

	len += strlen(str + len + 1) + 1;


	if (!strncmp(str + len + 1, "arena: ", 7)) {
		unsigned arena_len = strlen(str + len + 9);
		server->arena = (char *) malloc(sizeof(char) * (arena_len + 2));

		if (!server->arena)
			return 0;

		memcpy(server->arena, str + len + 8, arena_len + 1);
		server->arena[arena_len + 1] = '\0';
	}


	free(str);

	server->state = 1;

	return 1;
}

static int LoadServers()
// Get server list
{
#ifndef __WIN32__
	int s;
#else
	SOCKET s;
#endif

	char buf[1025];

	/* TODO: dodelat TCP makro */
	struct sockaddr_in server;
	char *master_server_ip;

	master_server_ip = gns_resolv(NET_MASTER_SERVER_DOMAIN);

	//printf("master_server_ip = %s\n", master_server_ip);

	if (master_server_ip == NULL)	// master server is down
	{
		return -1;
	}

	server.sin_family = AF_INET;
	server.sin_port = htons(NET_MASTER_PORT);
	server.sin_addr.s_addr = inet_addr(master_server_ip);

	free(master_server_ip);

	if ((s = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		return 0;

#ifndef __WIN32__
	// Lets make socket non-blocking
	int oldFlag = fcntl(s, F_GETFL, 0);
	if (fcntl(s, F_SETFL, oldFlag | O_NONBLOCK) == -1) {
		return -1;
	}
#else
	unsigned long arg = 1;
	// Operation is  FIONBIO. Parameter is pointer on non-zero number.
	if (ioctlsocket(s, FIONBIO, &arg) == SOCKET_ERROR) {
		WSACleanup();
		return -1;
	}
#endif

	if (connect(s, (struct sockaddr *) &server, sizeof(server)) == -1) {
#ifndef __WIN32__
		if (errno != EINPROGRESS)
			return -1;
#else
		if (WSAGetLastError() != WSAEWOULDBLOCK) {
			WSACleanup();
			return -1;
		}
#endif
	}

	struct timeval tv;

	fd_set myset;

	FD_ZERO(&myset);
	FD_SET(s, &myset);

	tv.tv_sec = 3;
	tv.tv_usec = 0;

	int ret = select(s + 1, NULL, &myset, NULL, &tv);

	if (ret == -1)
		return 0;

	if (ret == 0)
		return -2;

	/* send request for server list */
	send(s, "l", 1, 0);

	FD_ZERO(&myset);
	FD_SET(s, &myset);

	ret = select(s + 1, &myset, NULL, NULL, &tv);

	if (ret == -1)
		return 0;

	if (ret == 0)
		return -2;

	int len = recv(s, buf, 1024, 0);

#ifndef __WIN32__
	close(s);
#else
	closesocket(s);
	WSACleanup();
#endif

	if (len <= 0)
		return 0;

	int i = 0;
	char list[256];

	struct in_addr srv;

	typedef struct {
		unsigned port;
		unsigned ip;
	} response_head;

	while (1) {
		response_head *header = (response_head *) ((char *) buf + i);

		server_t *ctx;

		// alloc and init context
		ctx = (server_t *) malloc(sizeof(server_t));

		ctx->ip = header->ip;
		ctx->port = header->port;
		ctx->name = 0;
		ctx->version = 0;
		ctx->arena = 0;

		//int ret = server_getinfo(ctx);
		ret = server_getinfo(ctx);

		srv.s_addr = ctx->ip;

		char *address = (char *) inet_ntoa(srv);

		memset(list, ' ', 254);
		list[255] = '\0';

		if (ret == 1) {
			/*sprintf (list, "%s (%s) - %s:%d - %s - %d/%d - %dms", ctx->name ? ctx->name : "Unknown", ctx->version, address, ctx->port,
			   ctx->arena ? ctx->arena : "Unknown", ctx->clients, ctx->maxclients, ctx->ping); */

			if (ctx->name)
				memcpy(list, ctx->name, strlen(ctx->name));
			else
				memcpy(list, "Unknown", 7);

			memcpy(list + 30, ctx->version, strlen(ctx->version));

			unsigned a_len = strlen(address);
			memcpy(list + 38, address, a_len);
			memcpy(list + 38 + a_len, ":", 1);

			sprintf(buf, "%d", ctx->port);

			memcpy(list + 39 + a_len, buf, strlen(buf));

			memcpy(list + 60, ctx->arena, strlen(ctx->arena));

			sprintf(buf, "%d/%d", ctx->clients, ctx->maxclients);

			memcpy(list + 75, buf, strlen(buf));

			sprintf(buf, "%dms", ctx->ping);

			memcpy(list + 83, buf, strlen(buf));
		}

		select_add(select_server, list);

		// add into list
		ctx->next = &server_list;
		ctx->prev = server_list.prev;
		ctx->prev->next = ctx;
		ctx->next->prev = ctx;

		i += sizeof(response_head) + 1;

		if (buf[i - 1] != '\n' || (i + 1) > len)
			break;
	}

	return 1;
}

static int RefreshServers()
{
/*
	list_destroy_item(select_server->list, free);
	select_server->list = list_new();
*/
	select_remove_all(select_server);

	struct in_addr srv;

	server_t *server;
	for (server = server_list.next; server != &server_list;
		 server = server->next) {
		if (server->name)
			free(server->name);

		if (server->version)
			free(server->version);

		if (server->arena)
			free(server->arena);

		server->name = 0;
		server->version = 0;
		server->arena = 0;

		char list[256];
		char buf[32];

		int ret = server_getinfo(server);

		srv.s_addr = server->ip;

		char *address = (char *) inet_ntoa(srv);

		if (ret == 1) {
			memset(list, ' ', 254);
			list[255] = '\0';

			if (server->name)
				memcpy(list, server->name, strlen(server->name));
			else
				memcpy(list, "Unknown", 7);

			memcpy(list + 30, server->version, strlen(server->version));

			unsigned a_len = strlen(address);
			memcpy(list + 38, address, a_len);
			memcpy(list + 38 + a_len, ":", 1);

			sprintf(buf, "%d", server->port);

			memcpy(list + 39 + a_len, buf, strlen(buf));

			memcpy(list + 60, server->arena, strlen(server->arena));

			sprintf(buf, "%d/%d", server->clients, server->maxclients);

			memcpy(list + 75, buf, strlen(buf));

			sprintf(buf, "%dms", server->ping);

			memcpy(list + 83, buf, strlen(buf));
		}

		select_add(select_server, list);

	}

	return 1;
}

void browser_init()
{
	image_t *image;

	image = image_get(IMAGE_GROUP_BASE, "screen_main");
	image_backgorund = wid_image_new(0, 0, image);

	button_back =
		button_new(_("back"), 100, WINDOW_SIZE_Y - 100, eventWidget);
	button_play =
		button_new(_("Play"), WINDOW_SIZE_X - 200, WINDOW_SIZE_Y - 100,
						eventWidget);
	button_refresh =
		button_new(_("Refresh"), WINDOW_SIZE_X / 2 - 50,
						WINDOW_SIZE_Y - 100, eventWidget);

	label_server = label_new(_("server"), 120, 145, WIDGET_LABEL_LEFT);
	label_version = label_new(_("version"), 290, 145, WIDGET_LABEL_LEFT);
	label_address = label_new(_("IP"), 400, 145, WIDGET_LABEL_LEFT);
	label_arena = label_new(_("arena"), 550, 145, WIDGET_LABEL_LEFT);
	label_players = label_new(_("clients"), 645, 145, WIDGET_LABEL_LEFT);
	label_ping = label_new(_("ping"), 720, 145, WIDGET_LABEL_LEFT);

	select_server = select_new(50, label_server->y + 40, eventWidget);

	screen_register(screen_new
				   ("browser", browser_start, browser_event,
					browser_draw, browser_stop));

	server_list.next = &server_list;
	server_list.prev = &server_list;
}

void browser_quit()
{
	wid_image_destroy(image_backgorund);

	button_destroy(button_play);
	button_destroy(button_back);
	button_destroy(button_refresh);

	label_destroy(label_server);
	label_destroy(label_version);
	label_destroy(label_address);
	label_destroy(label_arena);
	label_destroy(label_players);
	label_destroy(label_ping);

	select_destroy(select_server);
}
