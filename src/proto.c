
#include <stdio.h>
#include <assert.h>

#include "list.h"
#include "tux.h"
#include "item.h"
#include "network.h"
#include "string_length.h"
#include "screen_world.h"
#include "screen_setting.h"
#include "screen_choiceArena.h"
#include "arenaFile.h"
#include "net_multiplayer.h"
#include "client.h"
#include "server.h"
#include "proto.h"

void proto_send_hello_client()
{
	char msg[STR_SIZE];
	
	strcpy(msg, "hello\n");
	sendServer(msg);
}

void proto_recv_hello_server(client_t *client, char *msg)
{
}

void proto_send_init_server(client_t *client)
{
	char msg[STR_SIZE];
	int n;

	assert( client != NULL );

	getSettingCountRound(&n);

	sprintf(msg, "init %d %d %d %d %s\n",
		client->tux->id, client->tux->x, client->tux->y,
		n, getArenaNetName( getChoiceArenaId() ));
	
	sendClient(client, msg);
}

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

void proto_send_event_server(tux_t *tux, int action, client_t *client)
{
	char msg[STR_SIZE];
	
	assert( tux != NULL );

	sprintf(msg, "event %d %d\n", tux->id, action);
	
	sendAllClientBut(msg, client);
}

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

	assert( client != NULL );
	assert( msg != NULL );

	sscanf(msg, "%s %d", cmd, &action);

	actionTux(client->tux, action);

	proto_send_event_server(client->tux, action, client);
}

void proto_send_newtux_server(client_t *client, tux_t *tux)
{
	char msg[STR_SIZE];

	assert( tux != NULL );

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

	assert( msg != NULL );

	sscanf(msg, "%s %d %d %d %d %d %d %s %d %d %d %d %d %d %d %d %d %d %d",
	cmd, &id, &x, &y, &position, &frame, &score, name,
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
	tux->status = TUX_STATUS_ALIVE;
}

void proto_send_kill_server(tux_t *tux)
{
	char msg[STR_SIZE];
	
	assert( tux != NULL );

	sprintf(msg, "kill %d\n", tux->id);
	
	sendAllClient(msg);
}

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

void proto_send_score_server(tux_t *tux)
{
	char msg[STR_SIZE];
	
	assert( tux != NULL );

	sprintf(msg, "score %d %d\n", tux->id, tux->score);
	
	sendAllClient(msg);
}

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

void proto_send_deltux_server(client_t *client)
{
	char msg[STR_SIZE];
	
	assert( client != NULL );

	sprintf(msg, "deltux %d\n", client->tux->id);

	sendAllClientBut(msg, client);
}

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

void proto_send_additem_server(item_t *p)
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

	sendAllClient(msg);
}

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

void proto_send_context_client(tux_t *tux)
{
	char msg[STR_SIZE];
	
	assert( tux != NULL );

	sprintf(msg, "context %s\n", tux->name);

	sendServer(msg);
}

void proto_recv_context_server(client_t *client, char *msg)
{
	char cmd[STR_SIZE];
	char name[STR_NAME_SIZE];

	assert( client != NULL );
	assert( msg != NULL );

	sscanf(msg, "%s %s\n", cmd, name);

	strcpy(client->tux->name, name);
	
	proto_send_newtux_server(NULL, client->tux);
}

void proto_send_ping_client()
{
	char msg[STR_SIZE];
	strcpy(msg, "ping\n");
	sendServer(msg);
}

void proto_recv_ping_server(client_t *client, char *msg)
{
	assert( msg != NULL );
	assert( client != NULL );
	
	client->lastPing = SDL_GetTicks();
}

void proto_send_end_client()
{
	char msg[STR_SIZE];
	strcpy(msg, "end\n");
	sendServer(msg);
}

void proto_recv_end_server(client_t *client, char *msg)
{
	assert( msg != NULL );
	assert( client != NULL );

	client->status = NET_STATUS_ZOMBIE;
}

void proto_send_end_server()
{
	char msg[STR_SIZE];
	strcpy(msg, "end\n");
	sendAllClient(msg);
}

void proto_recv_end_client(char *msg)
{
	assert( msg != NULL );

	setWorldEnd();
}

