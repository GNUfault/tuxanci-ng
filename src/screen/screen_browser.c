
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "base/main.h"
#include "base/list.h"
#include "base/textFile.h"
#include "base/homeDirector.h"
#include "base/net_multiplayer.h"

#include "client/language.h"
#include "client/interface.h"
#include "client/screen.h"
#include "client/image.h"

#ifdef SUPPORT_NET_UNIX_UDP
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include "net_unix/udp.h"

extern int errno;
#endif

#ifndef NO_SOUND
#include "audio/music.h"
#endif

#include "screen/screen_gameType.h"
#include "screen/screen_browser.h"

#include "widget/widget_image.h"
#include "widget/widget_label.h"
#include "widget/widget_button.h"
#include "widget/widget_select.h"

#define SERVER_TIMEOUT	200

static widget_image_t *image_backgorund;

static widget_button_t *button_play;
static widget_button_t *button_back;

static widget_label_t *label_server;

static widget_select_t *select_server;

static server_t server_list;

static int LoadServers ();
static server_t *server_getcurr ();

void startScreenBrowser()
{
#ifndef NO_SOUND
	playMusic("menu", MUSIC_GROUP_BASE);
#endif

	LoadServers ();
}

void drawScreenBrowser()
{
	drawWidgetImage(image_backgorund);

	drawWidgetLabel(label_server);

	drawWidgetSelect(select_server);

	drawWidgetButton(button_play);
	drawWidgetButton(button_back);
}

void eventScreenBrowser()
{
	eventWidgetSelect(select_server);

	eventWidgetButton(button_play);
	eventWidgetButton(button_back);
}

void stopScreenBrowser()
{
	unsigned i = 0;
	server_t *server;

	listDoEmpty(select_server->list);

	while (1) {
		i = 0;
		for (server = server_list.next; server != &server_list; server = server->next) {
			server->next->prev = server->prev;
			server->prev->next = server->next;

			//if (server->version)
			//	free (server->version);

			//if (server->arena)
				//free (server->arena);

			free (server);

			i ++;
			break;
		}

		if (!i)
			return;
	}
}

static void eventWidget(void *p)
{
	widget_button_t *button;
	
	button = (widget_button_t *)(p);

	if( button == button_play )
	{
		server_t *server = server_getcurr ();

		if (!server)
			return;

		/* server je offline */
		if (!server->state)
			return;
#ifdef SUPPORT_NET_UNIX_UDP
		struct sockaddr_in srv;
		srv.sin_addr.s_addr = server->ip;
#endif
		setSettingGameType (NET_GAME_TYPE_CLIENT);
		setSettingIP(inet_ntoa (srv.sin_addr));
		setSettingPort(server->port);

		setScreen("world");
	}

	if( button == button_back )
	{
		setScreen("gameType");
	}
}

server_t *server_getcurr ()
{
	unsigned i = 0;
	server_t *server;

	for (server = server_list.next; server != &server_list; server = server->next) {
		if (i == select_server->select)
			return server;

		i ++;
	}

	return 0;
}

int server_getinfo (server_t *server)
{
#ifdef SUPPORT_NET_UNIX_UDP
	struct sockaddr_in srv;
	int mySocket;

	if ((mySocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
	    return -1;
	}

	// Lets make socket non-blocking
	int oldFlag = fcntl (mySocket, F_GETFL, 0);
	if (fcntl (mySocket, F_SETFL, oldFlag | O_NONBLOCK) == -1) {
		return -1;
	}

	struct timeval tv;
	// Mno¾ina
	fd_set myset;
	// Mno¾ina obsahuje náhodná data. Odstraním je.
	FD_ZERO(&myset);
	// Zaplnìní mno¾iny sokety
	FD_SET(mySocket, &myset);
	// Vyplním èasový údaj (napøíklad na 3 minuty)
	tv.tv_sec = 2;// Poèet sekund
	tv.tv_usec = 0;// Poèet mikrosekund

	srv.sin_family = AF_INET;
	srv.sin_port = htons (server->port);
	srv.sin_addr.s_addr = server->ip;

	if (connect (mySocket, (struct sockaddr *) &srv, sizeof (srv)) == -1) {
		if (errno != EINPROGRESS)
		    return -1;
	}

	// Zavolám select (V Linuxu musím mít nastavenou promìnnou max.)
	int ret = select (mySocket+1, NULL, &myset, NULL, &tv);

	if (ret == -1)
		return 0;

	if (ret == 0)
		return -2;

	char request[8];
	memcpy (request, "status\n", 7);

	int r = send (mySocket, request, 7, 0);

	if (r == -1)
		return -2;

	FD_ZERO(&myset);
	FD_SET(mySocket, &myset);
	tv.tv_sec = 1;
	tv.tv_usec = 0;

	char *str = (char *) malloc (sizeof (char) * 257);

	if (!str)
		return -1;

	while (1) {
		int sel = select (mySocket + 1, &myset, NULL, NULL, &tv);

		if (sel == 0)
			return -2;

		if (sel == -1)
			return -2;
  
		if ((ret = recv(mySocket, str, 256, 0)) == -1)
			return -1;
    
		if (ret > 0)
			break;
	}

	close (mySocket);

	server->ping = 1000 - (tv.tv_usec/1000);
#else
	return 0;
#endif

	/*
		"version: svnX\n"
		"clients: N\n"
		"maxclients: M\n"
		"uptime: D\n"
	*/

	typedef struct {
		char *version;
		unsigned char clients;
		unsigned char maxclients;
		unsigned uptime;
	} server_info;

	server_info *info = (server_info *) malloc (sizeof (server_info));

	if (!info)
		return 0;

	unsigned i = 0;

	while (i < ret) {
		if (str[i] == '\n')
			str[i] = '\0';

		i ++;
	}

	unsigned len = strlen (str);
	if (!strncmp (str, "version: ", 9)) {
		unsigned ver_len = strlen (str+9);
		server->version = (char *) malloc (sizeof (char) * (ver_len + 1));
  
		if (!server->version)
			return 0;

		memcpy (server->version, str+9, ver_len+1);
	}

	if (!strncmp (str+len+1, "clients: ", 9))
		server->clients = atoi (str+len+10);

	len += strlen (str+len+1) + 1;

	if (!strncmp (str+len+1, "maxclients: ", 12))
		server->maxclients = atoi (str+len+13);

	len += strlen (str+len+1) + 1;

	if (!strncmp (str+len+1, "uptime: ", 8))
		server->uptime = atoi (str+len+9);

	len += strlen (str+len+1) + 1;

	if (!strncmp (str+len+1, "arena: ", 7)) {
		unsigned arena_len = strlen (str+9);
		server->arena = (char *) malloc (sizeof (char) * (arena_len + 1));
  
		if (!server->arena)
			return 0;

		memcpy (server->arena, str+len+8, arena_len+1);
	}

	free (str);

	server->state = 1;

	return 1;
}

static int LoadServers ()
// Get server list
{
	int s;
	char buf[1025];

	/* TODO: dodelat TCP makro */
#ifdef SUPPORT_NET_UNIX_UDP
	struct sockaddr_in server;
	
	server.sin_family = AF_INET;
	server.sin_port = htons (MASTER_PORT);
	server.sin_addr.s_addr = inet_addr (MASTER_SERVER);
	
	if ((s = socket (AF_INET, SOCK_STREAM, 0)) < 0) {
		close (s);
		return 0;
	}

	int oldFlag = fcntl (s, F_GETFL, 0);
	if (fcntl (s, F_SETFL, oldFlag | O_NONBLOCK) == -1) {
		return -1;
	}

	if (connect (s, (struct sockaddr *) &server, sizeof (server)) == -1) {
		if (errno != EINPROGRESS)
		    return -1;
	}

	struct timeval tv;
	// Mno¾ina
	fd_set myset;
	// Mno¾ina obsahuje náhodná data. Odstraním je.
	FD_ZERO(&myset);
	// Zaplnìní mno¾iny sokety
	FD_SET(s, &myset);
	// Vyplním èasový údaj (napøíklad na 3 minuty)
	tv.tv_sec = 3;// Poèet sekund
	tv.tv_usec = 0;// Poèet mikrosekund
	// Zavolám select (V Linuxu musím mít nastavenou promìnnou max.)
	int ret = select (s+1, NULL, &myset, NULL, &tv);

	if (ret == -1)
		return 0;

	if (ret == 0)
		return -2;

	/* send request for server list */
	send (s, "l", 1, 0);

	// Mno¾ina obsahuje náhodná data. Odstraním je.
	FD_ZERO(&myset);
	// Zaplnìní mno¾iny sokety
	FD_SET(s, &myset);

	ret = select (s+1, &myset, NULL, NULL, &tv);

	if (ret == -1)
		return 0;

	if (ret == 0)
		return -2;

	/* receive server list if exist */
	//int len;
	//do {
	int len = recv (s, buf, 1024, 0);
	//} while (len < 1);

	close (s);
#else
	return -1;
#endif

	if (len <= 0)
		return 0;

	unsigned i = 0;
	char list[256];

	struct sockaddr_in srv;

	typedef struct {
		unsigned port;
		unsigned ip;
	} response_head;

	while (1) {
		response_head *header = (response_head *) ((char *) buf + i);

		server_t *ctx;
			
		// alloc and init context
		ctx = (server_t *) malloc (sizeof (server_t));
		
		ctx->ip = header->ip;
		ctx->port = header->port;

		srv.sin_addr.s_addr = ctx->ip;

		int ret = server_getinfo (ctx);
  
		if (ret == 1) {
			sprintf (list, "%s:%d (%s) - %s - %d/%d - %dms", inet_ntoa (srv.sin_addr), ctx->port, ctx->version,
				ctx->arena, ctx->clients, ctx->maxclients, ctx->ping);
		} else if (ret == -2)
			sprintf (list, "%s:%d - Timeout", inet_ntoa (srv.sin_addr), ctx->port);
		else
			sprintf (list, "%s:%d - Offline", inet_ntoa (srv.sin_addr), ctx->port);

		addToWidgetSelect(select_server, list);

		// add into list
		ctx->next = &server_list;
		ctx->prev = server_list.prev;
		ctx->prev->next = ctx;
		ctx->next->prev = ctx;

		i += sizeof (response_head) + 1;

		if (buf[i-1] != '\n' || (i+1) > len)
			break;
	}

	return 1;
}

void initScreenBrowser()
{
	SDL_Surface *image;

	image = addImageData("screen_main.png", IMAGE_NO_ALPHA, "screen_browser", IMAGE_GROUP_BASE);
	image_backgorund  = newWidgetImage(0, 0, image);

	button_back = newWidgetButton(getMyText("BACK"), 100, WINDOW_SIZE_Y-100, eventWidget);
	button_play = newWidgetButton(getMyText("PLAY"), WINDOW_SIZE_X-200, WINDOW_SIZE_Y-100, eventWidget);;

	label_server = newWidgetLabel("Server list", 50, 145, WIDGET_LABEL_LEFT);

	select_server = newWidgetSelect(label_server->x, label_server->y+40, eventWidget);

	registerScreen( newScreen("browser", startScreenBrowser, eventScreenBrowser,
		drawScreenBrowser, stopScreenBrowser) );

	server_list.next = &server_list;
	server_list.prev = &server_list;
}

void quitScreenBrowser()
{
	destroyWidgetImage(image_backgorund);

	destroyWidgetButton(button_play);
	destroyWidgetButton(button_back);
	destroyWidgetLabel(label_server);
	destroyWidgetSelect(select_server);
}

