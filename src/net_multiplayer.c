
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>

#include "list.h"
#include "tux.h"
#include "item.h"
#include "network.h"
#include "buffer.h"
#include "string_length.h"
#include "screen.h"
#include "screen_world.h"
#include "screen_setting.h"
#include "net_multiplayer.h"

static int netGameType;
static int sock_server;

static list_t *listClient;
static buffer_t *clientBuffer;
static arena_t *arena;

int initServer(int port)
{
	sock_server = newSocket(port , NULL);
	
	if( sock_server < 0 )
	{
		return -1;
	}

	listClient = newList();
	netGameType = NET_GAME_TYPE_SERVER;

	printf("server listen port %d\n", port);

	return 0;
}

int initClient(int port, char *ip)
{
	sock_server = newSocket(port , ip);

	if( sock_server < 0 )
	{
		return -1;
	}

	clientBuffer = newBuffer( LIMIT_BUFFER );
	netGameType = NET_GAME_TYPE_CLIENT;

	printf("connect %s %d\n", ip, port);

	return 0;
}

void netSetArena(arena_t *p)
{
	arena = p;
}

int getNetTypeGame()
{
	return netGameType;
}

static void sendServer(char *msg)
{
	int ret;

	ret =  write(sock_server, msg, strlen(msg));

	if ( ret == 0 )
	{
		fprintf(stderr, "server uzatvoril sietovy socket\n");
		setScreen("analyze");
	}

	if ( ret < 0 )
	{
		fprintf(stderr, "nastala chyba pri poslani spravy serveru\n");
		setScreen("analyze");
	}
}

static void sendClient(client_t *p, char *msg)
{
	if( p->status == NET_STATUS_OK )
	{
		int ret;

		ret = write(p->socket, msg, strlen(msg));

		if( ret == 0 )
		{
			fprintf(stderr, "client sa odpojil\n");
			p->status = NET_STATUS_ZOMBIE;
		}

		if( ret < 0 )
		{
			fprintf(stderr, "nastala chyba pri posielanie spravy clientovy\n");
			p->status = NET_STATUS_ZOMBIE;
		}
	}
}

static void sendAllClientBut(char *msg, client_t *p)
{
	client_t *thisClient;
	int i;

	for( i = 0 ; i < listClient->count; i++)
	{
		thisClient = (client_t *) listClient->list[i];

		if( thisClient != p )
		{
			sendClient(thisClient, msg);
		}
	}
}

static void sendAllClient(char *msg)
{
	sendAllClientBut(msg, NULL);
}

static client_t* newClient(int sock)
{
	client_t *new;
	
	new = malloc( sizeof(client_t) );
	memset(new, 0, sizeof(client_t));
	
	new->socket = sock;
	new->status = NET_STATUS_OK;
	new->buffer = newBuffer(LIMIT_BUFFER);
	new->tux = newTux();
	new->tux->control = TUX_CONTROL_NET;
	addList(arena->listTux, new->tux);

	return new;
}

static void destroyClient(client_t *p)
{
	int index;

	close(p->socket);
	destroyBuffer(p->buffer);
	index = searchListItem(arena->listTux, p->tux);
	delListItem(arena->listTux, index, destroyTux);
	free(p);
}

void proto_send_init_server(client_t *client)
{
	char msg[STR_SIZE];

	sprintf(msg, "init %d %d %d\n",
	client->tux->id, client->tux->x, client->tux->y);
	
	sendClient(client, msg);
}

void proto_recv_init_client(char *msg)
{
	char cmd[STR_SIZE];
	tux_t *tux;

	tux = newTux();

	sscanf(msg, "%s %d %d %d\n",
	cmd, &tux->id, &tux->x, &tux->y);

	tux->control = TUX_CONTROL_KEYBOARD_RIGHT;
	getSettingNameRight(tux->name);
	proto_send_context_client(tux);
	addList(arena->listTux, tux);
}

void proto_send_event_server(tux_t *tux, int action, client_t *client)
{
	char msg[STR_SIZE];
	
	sprintf(msg, "event %d %d\n", tux->id, action);
	
	sendAllClientBut(msg, client);
}

void proto_recv_event_client(char *msg)
{
	char cmd[STR_SIZE];
	int id, action;
	tux_t *tux;

	sscanf(msg, "%s %d %d", cmd, &id, &action);

	tux = getTuxID(arena->listTux, id);

	if( tux != NULL )
	{
		actionTux(tux, action);
	}
}

void proto_send_event_client(int action)
{
	char msg[STR_SIZE];
	
	sprintf(msg, "event %d\n", action);
	
	sendServer(msg);
}

void proto_recv_event_server(client_t *client, char *msg)
{
	char cmd[STR_SIZE];
	int action;

	sscanf(msg, "%s %d", cmd, &action);

	actionTux(client->tux, action);

	proto_send_event_server(client->tux, action, client);
}

void proto_send_newtux_server(client_t *client, tux_t *tux)
{
	char msg[STR_SIZE];

	sprintf(msg, "newtux %d %d %d %d %d %d %s %d %d %d %d %d %d %d %d %d %d %d\n",
	tux->id ,tux->x, tux->y, tux->position ,tux->frame, tux->score, tux->name,
	tux->gun, tux->bonus, tux->shot[GUN_SIMPLE], tux->shot[GUN_DUAL_SIMPLE],
	tux->shot[GUN_SCATTER], tux->shot[GUN_TOMMY], tux->shot[GUN_LASSER],
	tux->shot[GUN_MINE], tux->shot[GUN_BOMBBALL],
	tux->bonus_time, tux->pickup_time);
	
	if( client == NULL )
	{
		sendAllClient(msg);
	}
	else
	{
		sendClient(client, msg);
	}
}

void proto_recv_newtux_client(char *msg)
{
	char cmd[STR_SIZE];
	char name[STR_NAME_SIZE];
	int id, x, y, position, frame, score;
	int myGun, myBonus;
	int  gun1, gun2, gun3, gun4, gun5, gun6, gun7;
	int time1, time2;
	tux_t *tux;

	sscanf(msg, "%s %d %d %d %d %d %d %s %d %d %d %d %d %d %d %d %d %d %d",
	cmd, &id, &x, &y, &position, &frame, &score, name,
	&myGun, &myBonus, &gun1, &gun2, &gun3, &gun4, &gun5, &gun6, &gun7, &time1, &time2);

	tux = getTuxID(arena->listTux, id);

	if( tux == NULL )
	{
		tux = newTux();
		tux->control = TUX_CONTROL_NET;
		addList(arena->listTux, tux);
	}

	tux->id = id;
	tux->x = x;
	tux->y = y;
	tux->position = position;
	tux->frame = frame;
	tux->frame = score;
	strcpy(tux->name, name);
	tux->gun = myGun;
	tux->bonus = myBonus;
	tux->shot[GUN_SIMPLE] = gun1;
	tux->shot[GUN_DUAL_SIMPLE] = gun2;
	tux->shot[GUN_SCATTER] = gun3;
	tux->shot[GUN_TOMMY] = gun4;
	tux->shot[GUN_LASSER] = gun5;
	tux->shot[GUN_MINE] = gun6;
	tux->shot[GUN_BOMBBALL] = gun7;
	tux->bonus_time = time1;
	tux->pickup_time = time2;
	tux->status = TUX_STATUS_ALIVE;
}

void proto_send_deltux_server(client_t *client)
{
	char msg[STR_SIZE];
	
	sprintf(msg, "deltux %d\n", client->tux->id);

	sendAllClientBut(msg, client);
}

void proto_recv_deltux_client(char *msg)
{
	char cmd[STR_SIZE];
	int id;
	tux_t *tux;

	sscanf(msg, "%s %d\n", cmd, &id);

	tux = getTuxID(arena->listTux, id);

	if( tux != NULL )
	{
		int index;
		
		index = searchListItem(arena->listTux, tux);
		delListItem(arena->listTux, index, destroyTux);
	}
}

void proto_send_additem_server(item_t *p)
{
	char msg[STR_SIZE];
	
	sprintf(msg, "additem %d %d %d %d %d\n", p->type, p->x, p->y, p->count, p->frame);

	sendAllClient(msg);
}

void proto_recv_additem_client(char *msg)
{
	char cmd[STR_SIZE];
	int type, x, y, count, frame;
	item_t *item;

	sscanf(msg, "%s %d %d %d %d %d\n", cmd, &type, &x, &y, &count, &frame);

	item = newItem(x, y, type);

	if( isConflictWithListItem(arena->listItem, item->x, item->y, item->w, item->h) )
	{
		destroyItem(item);
		return;
	}

	item->count = count;
	item->frame = frame;

	addList(arena->listItem, item);
}

void proto_send_context_client(tux_t *tux)
{
	char msg[STR_SIZE];
	
	sprintf(msg, "context %s\n", tux->name);

	sendServer(msg);
}

void proto_recv_context_server(client_t *client, char *msg)
{
	char cmd[STR_SIZE];
	char name[STR_NAME_SIZE];

	sscanf(msg, "%s %s\n", cmd, name);

	strcpy(client->tux->name, name);
	
	proto_send_newtux_server(NULL, client->tux);
}

void eventCreateNewClient(int sock)
{
	client_t *client;
	client_t *thisClient;
	tux_t *thisTux;
	item_t *thisItem;
	int i;

	client = newClient( getNewClient(sock) );
	addList(listClient, client);

	proto_send_init_server(client);

	proto_send_newtux_server(client, (tux_t *)(arena->listTux->list[0]) );

	for( i = 0 ; i < listClient->count; i++)
	{
		thisClient = (client_t *) listClient->list[i];
		thisTux = thisClient->tux;

		if( thisTux != client->tux )
		{
			proto_send_newtux_server(thisClient, client->tux);
			proto_send_newtux_server(client, thisTux);
		}
	}

	for( i = 0 ; i < arena->listItem->count; i++)
	{
		thisItem = (item_t *) arena->listItem->list[i];
		proto_send_additem_server(thisItem);
	}
}

static void eventClientSelect(client_t *client)
{
	char buffer[STR_SIZE];
	int ret;

	assert( client != NULL );

	memset(buffer, 0, STR_SIZE);
	ret = read(client->socket, buffer, STR_SIZE-1);

	if( ret <= 0 )
	{
		client->status = NET_STATUS_ZOMBIE;
		return;
	}

	if( addBuffer(client->buffer, buffer, ret) != 0 )
	{
		client->status = NET_STATUS_ZOMBIE;
		return;
	}
}

static void eventClientBuffer(client_t *client)
{
	char line[STR_SIZE];

	if( netGameType == NET_GAME_TYPE_NONE )
	{
		return;
	}

	/* obsluha udalosti od clientov */
	
	while( getBufferLine(client->buffer, line, STR_SIZE) >= 0 )
	{
		printf("dostal som %s", line);

		if( strncmp(line, "event", 5) == 0 )proto_recv_event_server(client, line);
		if( strncmp(line, "context", 7) == 0 )proto_recv_context_server(client, line);
	}
}

static void eventClientListBuffer()
{
	client_t *thisClient;
	int i;

	for( i = 0 ; i < listClient->count; i++)
	{
		thisClient = (client_t *) listClient->list[i];
		eventClientBuffer(thisClient);
	}
}

static void selectServerSocket()
{
	fd_set readfds;
	struct timeval tv;
	client_t *thisClient;
	int max_fd;
	int i;

	tv.tv_sec = 0;
	tv.tv_usec = 0;
	
	FD_ZERO(&readfds);
	FD_SET(sock_server, &readfds);
	max_fd = sock_server;

	for( i = 0 ; i < listClient->count; i++)
	{
		thisClient = (client_t *) listClient->list[i];

		FD_SET(thisClient->socket, &readfds);
	
		if( thisClient->socket > max_fd )
		{
			max_fd = thisClient->socket;
		}
	}

	select(max_fd+1, &readfds, (fd_set *)NULL, (fd_set *)NULL, &tv);

	if( FD_ISSET(sock_server, &readfds) )
	{
		eventCreateNewClient(sock_server);
	}

	for( i = 0 ; i < listClient->count; i++)
	{
		thisClient = (client_t *) listClient->list[i];

		if( FD_ISSET(thisClient->socket, &readfds) )
		{
			eventClientSelect(thisClient);
		}

		if( thisClient->status == NET_STATUS_ZOMBIE )
		{
			proto_send_deltux_server(thisClient);
			delListItem(listClient, i, destroyClient);
		}
	}
}

static void eventServerSelect(int sock)
{
	char buffer[STR_SIZE];
	int ret;

	memset(buffer,0 ,STR_SIZE);
	ret = read(sock, buffer, STR_SIZE-1);

	if( ret == 0 )
	{
		fprintf(stderr, "server uzatovril sietovy socket\n");
		setScreen("analyze");
		return;
	}

	if( ret < 0 )
	{
		fprintf(stderr, "chyba, spojenie prerusene \n");
		setScreen("analyze");
		return;
	}

	if( addBuffer(clientBuffer, buffer, ret) != 0 )
	{
		fprintf(stderr, "chyba, nemozem zapisovat do mojho buffera !\n");
		setScreen("analyze");
		return;
	}
}

static void eventServerBuffer()
{
	char line[STR_SIZE];

	if( netGameType == NET_GAME_TYPE_NONE )
	{
		return;
	}

	/* obsluha udalosti od servera */
	
	while ( getBufferLine(clientBuffer, line, STR_SIZE) >= 0 )
	{
		printf("dostal som %s", line);

		if( strncmp(line, "init", 4) == 0 )proto_recv_init_client(line);
		if( strncmp(line, "event", 5) == 0 )proto_recv_event_client(line);
		if( strncmp(line, "newtux", 6) == 0 )proto_recv_newtux_client(line);
		if( strncmp(line, "deltux", 6) == 0 )proto_recv_deltux_client(line);
		if( strncmp(line, "additem", 7) == 0 )proto_recv_additem_client(line);
	}
}

static void selectClientSocket()
{
	fd_set readfds;
	struct timeval tv;
	int max_fd;

	tv.tv_sec = 0;
	tv.tv_usec = 0;
	
	FD_ZERO(&readfds);
	FD_SET(sock_server, &readfds);
	max_fd = sock_server;

	select(max_fd+1, &readfds, (fd_set *)NULL, (fd_set *)NULL, &tv);

	if( FD_ISSET(sock_server, &readfds) )
	{
		eventServerSelect(sock_server);
	}
}

void eventNetMultiplayer()
{
	switch( netGameType )
	{
		case NET_GAME_TYPE_NONE :
		break;

		case NET_GAME_TYPE_SERVER :
			selectServerSocket();
			eventClientListBuffer();
		break;

		case NET_GAME_TYPE_CLIENT :
			selectClientSocket();
			eventServerBuffer();
		break;

		default :
			assert( ! "Premenna netGameType ma zlu hodnotu !" );
		break;
	}
}

static void quitServer()
{
	destroyListItem(listClient, destroyClient);
	close(sock_server);
	printf("quit port\n");
}

static void quitClient()
{
	destroyBuffer(clientBuffer);
	close(sock_server);
	printf("quit conenct\n");
}

void quitNetMultiplayer()
{
	switch( netGameType )
	{
		case NET_GAME_TYPE_NONE :
		break;

		case NET_GAME_TYPE_SERVER :
			quitServer();
		break;

		case NET_GAME_TYPE_CLIENT :
			quitClient();
		break;

		default :
			assert( ! "Premenna netGameType ma zlu hodnotu !" );
		break;
	}

	netGameType = NET_GAME_TYPE_NONE;
}
