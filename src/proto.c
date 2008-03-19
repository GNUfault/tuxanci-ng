
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "main.h"
#include "list.h"
#include "tux.h"
#include "item.h"
#include "shot.h"
#include "arenaFile.h"
#include "myTimer.h"
#include "server.h"
#include "proto.h"
#include "net_multiplayer.h"

#ifndef BUBLIC_SERVER
#include "network.h"
#include "screen_world.h"
#include "screen_setting.h"
#include "screen_choiceArena.h"
#include "client.h"
#endif

#ifdef BUBLIC_SERVER
#include "publicServer.h"
#endif

static void proto_send(int type, client_t *client, char *msg)
{
	assert( msg != NULL );

	switch( type )
	{
		case PROTO_SEND_ONE :
			assert( client != NULL );
			sendClient(client, msg);
#ifdef DEBUG_SERVER_SEND
			printf("send one client msg->%s", msg);
#endif
		break;
		case PROTO_SEND_ALL :
			assert( client == NULL );
			sendAllClient(msg);
#ifdef DEBUG_SERVER_SEND
			printf("send all clients msg->%s", msg);
#endif
		break;
		case PROTO_SEND_BUT :
			assert( client != NULL );
			sendAllClientBut(msg, client);
#ifdef DEBUG_SERVER_SEND
			printf("send but client msg->%s", msg);
#endif
		break;
		default :
			assert( ! "Premenna type ma zlu hodnotu !" );
		break;
	}
}

#ifndef BUBLIC_SERVER

void proto_send_hello_client()
{
	char msg[STR_SIZE];
	
	strcpy(msg, "hello\n");
	sendServer(msg);
}

#endif

void proto_recv_hello_server(client_t *client, char *msg)
{
}

void proto_send_init_server(int type, client_t *client, client_t *client2)
{
	char msg[STR_SIZE];
	int n = 9999;

	assert( client2 != NULL );

#ifndef BUBLIC_SERVER
	getSettingCountRound(&n);
#endif

	sprintf(msg, "init %d %d %d %d %s\n",
		client2->tux->id, client2->tux->x, client2->tux->y,
		n, getArenaNetName( getChoiceArenaId() ));
	
	proto_send(type, client, msg);
}

#ifndef BUBLIC_SERVER

void proto_recv_init_client(char *msg)
{
	char cmd[STR_SIZE];
	char arena_name[STR_SIZE];
	tux_t *tux;
	int id;
	int x, y, n;

	assert( msg != NULL );

	sscanf(msg, "%s %d %d %d %d %s\n",
	cmd, &id, &x, &y, &n, arena_name);

	setWorldArena( getArenaIdFormNetName(arena_name) );
	setMaxCountRound(n);

	tux = newTux();
	tux->id = id;
	tux->x = x;
	tux->y = y;
	tux->control = TUX_CONTROL_KEYBOARD_RIGHT;
	getSettingNameRight(tux->name);

	proto_send_context_client(tux);
	addList(getWorldArena()->listTux, tux);
}

#endif

void proto_send_event_server(int type, client_t *client, tux_t *tux, int action)
{
	char msg[STR_SIZE];
	
	assert( tux != NULL );

	sprintf(msg, "event %d %d\n", tux->id, action);
	
	proto_send(type, client, msg);
}

#ifndef BUBLIC_SERVER

void proto_recv_event_client(char *msg)
{
	char cmd[STR_SIZE];
	int id, action;
	tux_t *tux;

	assert( msg != NULL );

	sscanf(msg, "%s %d %d", cmd, &id, &action);

	tux = getTuxID(getWorldArena()->listTux, id);

	if( tux != NULL )
	{
		actionTux(tux, action);
	}
}

#endif

#ifndef BUBLIC_SERVER

void proto_send_event_client(int action)
{
	char msg[STR_SIZE];
	
	sprintf(msg, "event %d\n", action);
	
	sendServer(msg);
}

#endif

void proto_recv_event_server(client_t *client, char *msg)
{
	char cmd[STR_SIZE];
	int action;

	assert( client != NULL );
	assert( msg != NULL );

	sscanf(msg, "%s %d", cmd, &action);

	actionTux(client->tux, action);

	proto_send_event_server(PROTO_SEND_BUT, client, client->tux, action);
}

void proto_send_newtux_server(int type, client_t *client, tux_t *tux)
{
	char msg[STR_SIZE];

	assert( tux != NULL );

	sprintf(msg, "newtux %d %d %d %d %d %d %d %s %d %d %d %d %d %d %d %d %d %d %d\n",
	tux->id ,tux->x, tux->y, tux->status, tux->position ,tux->frame, tux->score, tux->name,
	tux->gun, tux->bonus, tux->shot[GUN_SIMPLE], tux->shot[GUN_DUAL_SIMPLE],
	tux->shot[GUN_SCATTER], tux->shot[GUN_TOMMY], tux->shot[GUN_LASSER],
	tux->shot[GUN_MINE], tux->shot[GUN_BOMBBALL],
	tux->bonus_time, tux->pickup_time);
	
	proto_send(type, client, msg);
}

#ifndef BUBLIC_SERVER

void proto_recv_newtux_client(char *msg)
{
	char cmd[STR_SIZE];
	char name[STR_NAME_SIZE];
	int id, x, y, status, position, frame, score;
	int myGun, myBonus;
	int  gun1, gun2, gun3, gun4, gun5, gun6, gun7;
	int time1, time2;
	tux_t *tux;

	assert( msg != NULL );

	sscanf(msg, "%s %d %d %d %d %d %d %d %s %d %d %d %d %d %d %d %d %d %d %d",
	cmd, &id, &x, &y, &status, &position, &frame, &score, name,
	&myGun, &myBonus, &gun1, &gun2, &gun3, &gun4, &gun5, &gun6, &gun7, &time1, &time2);

	tux = getTuxID(getWorldArena()->listTux, id);

	if( tux == NULL )
	{
		tux = newTux();
		tux->control = TUX_CONTROL_NET;
		addList(getWorldArena()->listTux, tux);
	}

	tux->id = id;
	tux->x = x;
	tux->y = y;
	tux->status = status;
	tux->position = position;
	tux->frame = frame;
	tux->score = score;
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
}

#endif

void proto_send_kill_server(int type, client_t *client, tux_t *tux)
{
	char msg[STR_SIZE];
	
	assert( tux != NULL );

	sprintf(msg, "kill %d\n", tux->id);
	
	proto_send(type, client, msg);
}

#ifndef BUBLIC_SERVER

void proto_recv_kill_client(char *msg)
{
	char cmd[STR_SIZE];
	int id;
	tux_t *tux;

	assert( msg != NULL );

	sscanf(msg, "%s %d", cmd, &id);

	tux = getTuxID(getWorldArena()->listTux, id);

	if( tux != NULL )
	{
		eventTuxIsDead(tux);
	}
}

#endif

void proto_send_score_server(int type, client_t *client, tux_t *tux)
{
	char msg[STR_SIZE];
	
	assert( tux != NULL );

	sprintf(msg, "score %d %d\n", tux->id, tux->score);
	
	proto_send(type, client, msg);
}

#ifndef BUBLIC_SERVER

void proto_recv_score_client(char *msg)
{
	char cmd[STR_SIZE];
	int id, score;
	tux_t *tux;

	assert( msg != NULL );

	sscanf(msg, "%s %d %d", cmd, &id, &score);

	tux = getTuxID(getWorldArena()->listTux, id);

	if( tux != NULL )
	{
		tux->score = score;
	}

	countRoundInc();
}

#endif

void proto_send_deltux_server(int type, client_t *client, client_t *client2)
{
	char msg[STR_SIZE];
	
	assert( client2 != NULL );

	sprintf(msg, "deltux %d\n", client2->tux->id);

	proto_send(type, client, msg);
}

#ifndef BUBLIC_SERVER

void proto_recv_deltux_client(char *msg)
{
	char cmd[STR_SIZE];
	int id;
	tux_t *tux;

	assert( msg != NULL );

	sscanf(msg, "%s %d\n", cmd, &id);

	tux = getTuxID(getWorldArena()->listTux, id);

	if( tux != NULL )
	{
		int index;
		
		index = searchListItem(getWorldArena()->listTux, tux);
		delListItem(getWorldArena()->listTux, index, destroyTux);
	}
}

#endif

void proto_send_additem_server(int type, client_t *client, item_t *p)
{
	char msg[STR_SIZE];
	int id = -1;

	assert( p != NULL );

	if( p->author != NULL )
	{
		id = p->author->id;
	}

	sprintf(msg, "additem %d %d %d %d %d %d\n",
		p->type, p->x, p->y, p->count, p->frame, id);

	proto_send(type, client, msg);
}

#ifndef BUBLIC_SERVER

void proto_recv_additem_client(char *msg)
{
	char cmd[STR_SIZE];
	int type, x, y, count, frame, id;
	item_t *item;

	assert( msg != NULL );

	sscanf(msg, "%s %d %d %d %d %d %d\n", cmd, &type, &x, &y, &count, &frame, &id);

	item = newItem(x, y, type, getTuxID(getWorldArena()->listTux, id));

	if( isConflictWithListItem(getWorldArena()->listItem, item->x, item->y, item->w, item->h) )
	{
		destroyItem(item);
		return;
	}

	item->count = count;
	item->frame = frame;

	addList(getWorldArena()->listItem, item);
}

#endif

void proto_send_shot_server(int type, client_t *client, shot_t *p)
{
	char msg[STR_SIZE];
	int id = -1;

	assert( p != NULL );

	if( p->author != NULL )
	{
		id = p->author->id;
	}

	sprintf(msg, "shot %d %d %d %d %d %d %d %d\n",
		p->x, p->y, p->px, p->py, p->position, p->gun, id, p->isCanKillAuthor);

	proto_send(type, client, msg);
}

#ifndef BUBLIC_SERVER

void proto_recv_shot_client(char *msg)
{
	char cmd[STR_SIZE];
	int x, y, px, py, position, gun, id, isCanKillAuthor;
	shot_t *shot;

	assert( msg != NULL );

	sscanf(msg, "%s %d %d %d %d %d %d %d %d\n",
		cmd, &x, &y, &px, &py, &position, &gun, &id, &isCanKillAuthor);

	shot = newShot(x, y, px, py, gun, getTuxID(getWorldArena()->listTux, id));

	shot->isCanKillAuthor = isCanKillAuthor;
	shot->position = position;

	if( shot->gun == GUN_LASSER )
	{
		printf("transform\n");
		transformOnlyLasser(shot);
	}

	addList(getWorldArena()->listShot, shot);
}

#endif

#ifndef BUBLIC_SERVER

void proto_send_context_client(tux_t *tux)
{
	char msg[STR_SIZE];
	
	assert( tux != NULL );

	sprintf(msg, "context %s\n", tux->name);

	sendServer(msg);
}

#endif

void proto_recv_context_server(client_t *client, char *msg)
{
	char cmd[STR_SIZE];
	char name[STR_NAME_SIZE];

	assert( client != NULL );
	assert( msg != NULL );

	sscanf(msg, "%s %s\n", cmd, name);

	strcpy(client->tux->name, name);
	
//	proto_send_newtux_server(NULL, client->tux);
}

#ifndef BUBLIC_SERVER

void proto_send_ping_client()
{
	char msg[STR_SIZE];
	strcpy(msg, "ping\n");
	sendServer(msg);
}

#endif

void proto_recv_ping_server(client_t *client, char *msg)
{
	assert( msg != NULL );
	assert( client != NULL );
	
#if defined SUPPORT_NET_UNIX_UDP || defined SUPPORT_NET_SDL_UDP
	client->lastPing = getMyTime();
#endif
}

#ifndef BUBLIC_SERVER

void proto_send_end_client()
{
	char msg[STR_SIZE];
	strcpy(msg, "end\n");
	sendServer(msg);
}

#endif

void proto_recv_end_server(client_t *client, char *msg)
{
	assert( msg != NULL );
	assert( client != NULL );

	client->status = NET_STATUS_ZOMBIE;
}

void proto_send_ping_server(int type, client_t *client)
{
	char msg[STR_SIZE];
	strcpy(msg, "ping\n");
	proto_send(type, client, msg);
}

#ifndef BUBLIC_SERVER

void proto_recv_ping_client(char *msg)
{
	assert( msg != NULL );
	
#if defined SUPPORT_NET_UNIX_UDP || defined SUPPORT_NET_SDL_UDP
	refreshPingServerAlive();
#endif
}

#endif

void proto_send_end_server(int type, client_t *client)
{
	char msg[STR_SIZE];
	strcpy(msg, "end\n");
	proto_send(type, client, msg);
}


#ifndef BUBLIC_SERVER

void proto_recv_end_client(char *msg)
{
	assert( msg != NULL );

	setWorldEnd();
}

#endif
