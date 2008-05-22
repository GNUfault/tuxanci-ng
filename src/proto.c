
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

#ifndef PUBLIC_SERVER
#include "network.h"
#include "screen_world.h"
#include "screen_setting.h"
#include "screen_choiceArena.h"
#include "client.h"
#include "term.h"
#endif

#ifdef PUBLIC_SERVER
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
			printf("send one client msg->%s\n", msg);
#endif
		break;
		case PROTO_SEND_ALL :
			assert( client == NULL );
			sendAllClient(msg);
#ifdef DEBUG_SERVER_SEND
			printf("send all clients msg->%s\n", msg);
#endif
		break;
		case PROTO_SEND_BUT :
			assert( client != NULL );
			sendAllClientBut(msg, client);
#ifdef DEBUG_SERVER_SEND
			printf("send but client msg->%s\n", msg);
#endif
		break;
		default :
			assert( ! "Premenna type ma zlu hodnotu !" );
		break;
	}
}

void proto_send_error_server(int type, client_t *client, int errorcode)
{
	char msg[STR_PROTO_SIZE];
	
	sprintf(msg, "error %d\n", errorcode);

	proto_send(type, client, msg);
}

#ifndef PUBLIC_SERVER

void proto_recv_error_client(char *msg)
{
	char cmd[STR_PROTO_SIZE];
	int errorcode;

	assert( msg != NULL );

	sscanf(msg, "%s %d",
		cmd, &errorcode);

	printf("proto error code %d\n", errorcode);
}

#endif

#ifndef PUBLIC_SERVER

void proto_send_hello_client(char *name)
{
	char msg[STR_PROTO_SIZE];
	
	sprintf(msg, "hello %s %s\n", TUXANCI_NG_VERSION, name);
	sendServer(msg);
}

#endif

void proto_recv_hello_server(client_t *client, char *msg)
{
	char cmd[STR_PROTO_SIZE];
	char version[STR_NAME_SIZE];
	char name[STR_NAME_SIZE];
	
	assert( client != NULL );
	
	strcpy(version, "");
	strcpy(name, "");
	
	sscanf(msg, "%s %s %s", cmd, version, name);
	
	if( strncmp(version, TUXANCI_NG_VERSION, strlen(TUXANCI_NG_VERSION)) != 0 )
	{
		proto_send_error_server(PROTO_SEND_ONE, client, PROTO_ERROR_CODE_BAD_VERSION);
		client->status = NET_STATUS_ZOMBIE;
		return;
	}

	client->tux = newTux();
	client->tux->control = TUX_CONTROL_NET;
	addList(getCurrentArena()->listTux, client->tux);

	if( strlen(name) > STR_NAME_SIZE-1 )
	{
		name[STR_NAME_SIZE-1] = '\0';
	}

	strcpy(client->tux->name, name);
	sendInfoCreateClient(client);
}

void proto_send_status_server(int type, client_t *client)
{
	char msg[STR_PROTO_SIZE];
	
	sprintf(msg,	"version: %s\n"
			"clients: %d\n"
			"maxclients: %d\n"
			"uptime: %d\n"
			"arena: %s\n",
	TUXANCI_NG_VERSION, getCurrentArena()->listTux->count,
	getServerMaxClients(), (unsigned int)getUpdateServer(),
	getArenaNetName( getChoiceArenaId() ) );

	proto_send(type, client, msg);
}

void proto_recv_status_server(client_t *client, char *msg)
{
	proto_send_status_server(PROTO_SEND_ONE, client);
}

void proto_send_init_server(int type, client_t *client, client_t *client2)
{
	char msg[STR_PROTO_SIZE];
	int n = WORLD_COUNT_ROUND_UNLIMITED;

	assert( client2 != NULL );

#ifndef PUBLIC_SERVER
	getSettingCountRound(&n);
#endif

	sprintf(msg, "init %d %d %d %d %s\n",
		client2->tux->id, client2->tux->x, client2->tux->y,
		n, getArenaNetName( getChoiceArenaId() ));
	
	proto_send(type, client, msg);
}

#ifndef PUBLIC_SERVER

void proto_recv_init_client(char *msg)
{
	char cmd[STR_PROTO_SIZE];
	char arena_name[STR_PROTO_SIZE];
	tux_t *tux;
	int id;
	int x, y, n;

	assert( msg != NULL );

	sscanf(msg, "%s %d %d %d %d %s",
	cmd, &id, &x, &y, &n, arena_name);

	setWorldArena( getArenaIdFormNetName(arena_name) );
	setMaxCountRound(n);

	tux = newTux();
	replaceTuxID(tux, id);
	tux->x = x;
	tux->y = y;
	tux->control = TUX_CONTROL_KEYBOARD_RIGHT;

	getSettingNameRight(tux->name);

	addList(getCurrentArena()->listTux, tux);
}

#endif

void proto_send_event_server(int type, client_t *client, tux_t *tux, int action)
{
	char msg[STR_PROTO_SIZE];
	
	assert( tux != NULL );

	sprintf(msg, "event %d %d\n", tux->id, action);
	
	proto_send(type, client, msg);
}

#ifndef PUBLIC_SERVER

void proto_recv_event_client(char *msg)
{
	char cmd[STR_PROTO_SIZE];
	int id, action;
	tux_t *tux;

	assert( msg != NULL );

	sscanf(msg, "%s %d %d", cmd, &id, &action);

	tux = getTuxID(getCurrentArena()->listTux, id);

	if( tux != NULL )
	{
		actionTux(tux, action);
	}
}

#endif

#ifndef PUBLIC_SERVER

void proto_send_event_client(int action)
{
	char msg[STR_PROTO_SIZE];
	
	sprintf(msg, "event %d\n", action);
	
	sendServer(msg);
}

#endif

void proto_recv_event_server(client_t *client, char *msg)
{
	char cmd[STR_PROTO_SIZE];
	int action;

	assert( client != NULL );
	assert( msg != NULL );


	sscanf(msg, "%s %d", cmd, &action);

	proto_send_event_server(PROTO_SEND_BUT, client, client->tux, action);

	actionTux(client->tux, action);

}

void proto_send_newtux_server(int type, client_t *client, tux_t *tux)
{
	char msg[STR_PROTO_SIZE];
	int x, y;

	assert( tux != NULL );

	if( tux->bonus == BONUS_HIDDEN )
	{
		x = TUX_LOCATE_UNKNOWN;
		y = TUX_LOCATE_UNKNOWN;
	}
	else
	{
		x = tux->x;
		y = tux->y;
	}

	sprintf(msg, "newtux %d %d %d %d %d %d %d %s %d %d %d %d %d %d %d %d %d %d %d\n",
	tux->id ,x, y, tux->status, tux->position ,tux->frame, tux->score, tux->name,
	tux->gun, tux->bonus, tux->shot[GUN_SIMPLE], tux->shot[GUN_DUAL_SIMPLE],
	tux->shot[GUN_SCATTER], tux->shot[GUN_TOMMY], tux->shot[GUN_LASSER],
	tux->shot[GUN_MINE], tux->shot[GUN_BOMBBALL],
	tux->bonus_time, tux->pickup_time);
	
	proto_send(type, client, msg);
}

#ifndef PUBLIC_SERVER

void proto_recv_newtux_client(char *msg)
{
	char cmd[STR_PROTO_SIZE];
	char name[STR_NAME_SIZE];
	int id, x, y, status, position, frame, score;
	int myGun, myBonus;
	int  gun1, gun2, gun3, gun4, gun5, gun6, gun7;
	int time1, time2;
	tux_t *tux;

	assert( msg != NULL );

	if( getCurrentArena() == NULL )
	{
		return;
	}

	sscanf(msg, "%s %d %d %d %d %d %d %d %s %d %d %d %d %d %d %d %d %d %d %d",
	cmd, &id, &x, &y, &status, &position, &frame, &score, name,
	&myGun, &myBonus, &gun1, &gun2, &gun3, &gun4, &gun5, &gun6, &gun7, &time1, &time2);

	tux = getTuxID(getCurrentArena()->listTux, id);

	if( tux == NULL )
	{
		char term_msg[STR_SIZE];
		sprintf(term_msg, "connect new client( id = %d)\n", id);
		appendTextInTerm(term_msg);

		tux = newTux();
		tux->control = TUX_CONTROL_NET;
		addList(getCurrentArena()->listTux, tux);
	}

	replaceTuxID(tux, id);
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
	char msg[STR_PROTO_SIZE];
	
	assert( tux != NULL );

	sprintf(msg, "kill %d\n", tux->id);
	
	proto_send(type, client, msg);
}

#ifndef PUBLIC_SERVER

void proto_recv_kill_client(char *msg)
{
	char cmd[STR_PROTO_SIZE];
	int id;
	tux_t *tux;

	assert( msg != NULL );

	sscanf(msg, "%s %d", cmd, &id);

	tux = getTuxID(getCurrentArena()->listTux, id);

	if( tux != NULL )
	{
		eventTuxIsDead(tux);
	}
}

#endif

void proto_send_score_server(int type, client_t *client, tux_t *tux)
{
	char msg[STR_PROTO_SIZE];
	
	assert( tux != NULL );

	sprintf(msg, "score %d %d\n", tux->id, tux->score);
	
	proto_send(type, client, msg);
}

#ifndef PUBLIC_SERVER

void proto_recv_score_client(char *msg)
{
	char cmd[STR_PROTO_SIZE];
	int id, score;
	tux_t *tux;

	assert( msg != NULL );

	sscanf(msg, "%s %d %d", cmd, &id, &score);

	tux = getTuxID(getCurrentArena()->listTux, id);

	if( tux != NULL )
	{
		char term_msg[STR_SIZE];
		sprintf(term_msg, "tux with id %d set score to %d\n", tux->id, score);
		appendTextInTerm(term_msg);

		tux->score = score;
	}

	countRoundInc();
}

#endif

void proto_send_deltux_server(int type, client_t *client, client_t *client2)
{
	char msg[STR_PROTO_SIZE];
	
	assert( client2 != NULL );

	sprintf(msg, "deltux %d\n", client2->tux->id);

	proto_send(type, client, msg);
}

#ifndef PUBLIC_SERVER

void proto_recv_deltux_client(char *msg)
{
	char cmd[STR_PROTO_SIZE];
	int id;
	tux_t *tux;

	assert( msg != NULL );

	sscanf(msg, "%s %d", cmd, &id);

	tux = getTuxID(getCurrentArena()->listTux, id);

	if( tux != NULL )
	{
		char term_msg[STR_SIZE];
		int index;
	
		sprintf(term_msg, "tux with id %d is disconnect\n", tux->id);
		appendTextInTerm(term_msg);

		index = searchListItem(getCurrentArena()->listTux, tux);
		delListItem(getCurrentArena()->listTux, index, destroyTux);
	}
}

#endif

void proto_send_additem_server(int type, client_t *client, item_t *p)
{
	char msg[STR_PROTO_SIZE];

	assert( p != NULL );

	sprintf(msg, "additem %d %d %d %d %d %d %d\n",
		p->id, p->type, p->x, p->y, p->count, p->frame, p->author_id);

	proto_send(type, client, msg);
}

#ifndef PUBLIC_SERVER

void proto_recv_additem_client(char *msg)
{
	char cmd[STR_PROTO_SIZE];
	int id, type, x, y, count, frame, author_id;
	item_t *item;

	assert( msg != NULL );

	if( getCurrentArena() == NULL )
	{
		return;
	}

	sscanf(msg, "%s %d %d %d %d %d %d %d", cmd, &id, &type, &x, &y, &count, &frame, &author_id);

	if( ( item = getItemID(getCurrentArena()->listItem, id) ) != NULL )
	{
		item->lastSync = getMyTime();
		return;
	}

	item = newItem(x, y, type, author_id);
/*
	if( isConflictWithListItem(getCurrentArena()->listItem, item->x, item->y, item->w, item->h) )
	{
		destroyItem(item);
		return;
	}
*/
	replaceItemID(item, id);
	item->count = count;
	item->frame = frame;
	item->lastSync = getMyTime();

	addList(getCurrentArena()->listItem, item);
}

#endif

void proto_send_item_server(int type, client_t *client, tux_t *tux, item_t *item)
{
	char msg[STR_PROTO_SIZE];
	int tux_id = -1;

	assert( item != NULL );

	if( tux != NULL )
	{
		tux_id = tux->id;
	}

	sprintf(msg, "item %d %d\n",
		tux_id, item->id);

	proto_send(type, client, msg);
}

#ifndef PUBLIC_SERVER

void proto_recv_item_client(char *msg)
{
	char cmd[STR_PROTO_SIZE];
	int tux_id, item_id;
	arena_t *arena;

	item_t *item;
	tux_t *tux;

	assert( msg != NULL );

	sscanf(msg, "%s %d %d", cmd, &tux_id, &item_id);

	arena = getCurrentArena();

	item = getItemID(arena->listItem, item_id);
	tux = getTuxID(arena->listTux, tux_id);

	if( item == NULL)
	{
		return;
	}

	if( tux != NULL )
	{
		eventGiveTuxItem(tux, arena->listItem, item);
	}
	else
	{
		mineExplosion(arena->listItem, item);
	}
}

#endif

void proto_send_shot_server(int type, client_t *client, shot_t *p)
{
	char msg[STR_PROTO_SIZE];

	assert( p != NULL );

	sprintf(msg, "shot %d %d %d %d %d %d %d %d %d\n",
		p->id, p->x, p->y, p->px, p->py, p->position, p->gun, p->author_id, p->isCanKillAuthor);

	proto_send(type, client, msg);
}

#ifndef PUBLIC_SERVER

void proto_recv_shot_client(char *msg)
{
	char cmd[STR_PROTO_SIZE];
	int x, y, px, py, position, gun, shot_id, author_id, isCanKillAuthor;
	shot_t *shot;

	assert( msg != NULL );

	sscanf(msg, "%s %d %d %d %d %d %d %d %d %d",
		cmd, &shot_id, &x, &y, &px, &py, &position, &gun, &author_id, &isCanKillAuthor);

/*
	if( getShotID(getCurrentArena()->listShot, shot_id) != NULL )
	{
		return;
	}
*/
	shot = newShot(x, y, px, py, gun, author_id);

	replaceShotID(shot, shot_id);
	shot->isCanKillAuthor = isCanKillAuthor;
	shot->position = position;

/*
	if( shot->gun == GUN_LASSER )
	{
		transformOnlyLasser(shot);
	}
*/
	addList(getCurrentArena()->listShot, shot);
}

#endif

void proto_send_delshot_server(int type, client_t *client, shot_t *shot)
{
	char msg[STR_PROTO_SIZE];

	assert( shot != NULL );

	sprintf(msg, "delshot %d\n", shot->id);

	proto_send(type, client, msg);
}

#ifndef PUBLIC_SERVER

void proto_recv_delshot_client(char *msg)
{
	char cmd[STR_PROTO_SIZE];
	shot_t *shot;
	int id;

	assert( msg != NULL );

	sscanf(msg, "%s %d",
		cmd, &id);

	shot = getShotID(getCurrentArena()->listShot, id);

	if( shot != NULL )
	{
		int index;

		index = searchListItem(getCurrentArena()->listShot, shot);

		if( index >= 0 )
		{
			delListItem(getCurrentArena()->listShot, index, destroyShot);
		}
	}
}

#endif

#ifndef PUBLIC_SERVER

void proto_send_ping_client()
{
	char msg[STR_PROTO_SIZE];
	sprintf(msg, "ping\n");
	sendServer(msg);
}

#endif

void proto_recv_ping_server(client_t *client, char *msg)
{
}

void proto_send_ping_server(int type, client_t *client)
{
	char msg[STR_PROTO_SIZE];
	sprintf(msg, "ping\n");
	proto_send(type, client, msg);
}

#ifndef PUBLIC_SERVER

void proto_recv_ping_client(char *msg)
{
}

#endif

void proto_send_end_server(int type, client_t *client)
{
	char msg[STR_PROTO_SIZE];
	strcpy(msg, "end\n");
	proto_send(type, client, msg);
}

#ifndef PUBLIC_SERVER

void proto_recv_end_client(char *msg)
{
	assert( msg != NULL );

	setWorldEnd();
}

#endif

#ifndef PUBLIC_SERVER

void proto_send_end_client()
{
	char msg[STR_PROTO_SIZE];
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

