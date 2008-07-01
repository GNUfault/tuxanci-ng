
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "base/main.h"
#include "base/list.h"
#include "base/tux.h"
#include "base/item.h"
#include "base/shot.h"
#include "base/arenaFile.h"
#include "base/myTimer.h"
#include "base/server.h"
#include "base/proto.h"
#include "base/net_multiplayer.h"
#include "base/checkFront.h"
#include "base/idManager.h"
#include "base/protect.h"

#ifndef PUBLIC_SERVER
#include "client/language.h"
#include "screen/screen_world.h"
#include "screen/screen_setting.h"
#include "screen/screen_choiceArena.h"
#include "screen/screen_analyze.h"
#include "client/client.h"
#include "client/term.h"
#endif

#ifdef PUBLIC_SERVER
#include "server/heightScore.h"
#include "server/publicServer.h"
#include "server/serverConfigFile.h"
#endif

void proto_send_error_server(int type, client_t *client, int errorcode)
{
	char msg[STR_PROTO_SIZE];
	
	sprintf(msg, "error %d\n", errorcode);

	protoSendClient(type, client, msg, CHECK_FORNT_TYPE_SIMPLE, CHECK_FRONT_ID_NONE);
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

	switch(errorcode)
	{
		case PROTO_ERROR_CODE_BAD_VERSION :
			setMsgToAnalyze(getMyText("ERROR_BAD_VERSION"));
			setWorldEnd();
		break;
	}
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
	client->tux->client = client;

	addObjectToSpace(getCurrentArena()->spaceTux, client->tux);

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
	char *name;
	char *version;
	int clients;
	int maxclients;
	unsigned int uptime;
	char *arena;

#ifndef PUBLIC_SERVER
	name = "noname";
#endif

#ifdef PUBLIC_SERVER
	name = getServerConfigFileValue("NAME", "noname");
#endif

	version = TUXANCI_NG_VERSION;
	clients = getCurrentArena()->spaceTux->list->count;
	maxclients = getServerMaxClients();
	uptime = (unsigned int)getUpdateServer();
	arena = getArenaNetName( getChoiceArenaId() );

	sprintf(msg,	"name: %s\n"
			"version: %s\n"
			"clients: %d\n"
			"maxclients: %d\n"
			"uptime: %d\n"
			"arena: %s\n",

			name, version, clients,
			maxclients, uptime, arena);

	protoSendClient(type, client, msg, CHECK_FORNT_TYPE_SIMPLE, CHECK_FRONT_ID_NONE);
	//proto_send(type, client, msg);
}

void proto_recv_status_server(client_t *client, char *msg)
{
	proto_send_status_server(PROTO_SEND_ONE, client);
}

#ifdef PUBLIC_SERVER

void proto_send_listscore_server(int type, client_t *client, int max)
{
	char *str;
	char *msg;
	int i;

	msg = malloc( (max * 48) * sizeof(char) );
	strcpy(msg, "");

	for( i = 0 ; i < max ; i++ )
	{
		str  = getTableItem(i);

		if( str == NULL )
		{
			break;
		}

		strcat(msg, str);
		strcat(msg, "\n");
	}

	//proto_send(type, client, msg);
	protoSendClient(type, client, msg, CHECK_FORNT_TYPE_SIMPLE, CHECK_FRONT_ID_NONE);
	free(msg);
}

void proto_recv_listscore_server(client_t *client, char *msg)
{
	char cmd[STR_PROTO_SIZE];
	int max;
	int ret;

	assert( msg != NULL );

	ret = sscanf(msg, "%s %d",
		cmd, &max);

	if( ret != 2 )
	{
		max = HEIGHTSCORE_MAX_PLAYERS;
	}

	proto_send_listscore_server(PROTO_SEND_ONE, client, max);
}

#endif

#ifndef PUBLIC_SERVER

void proto_send_check_client(int id)
{
	char msg[STR_PROTO_SIZE];
	
	sprintf(msg, "check %d\n", id);
	sendServer(msg);
}

#endif

void proto_recv_check_server(client_t *client, char *msg)
{
	char cmd[STR_PROTO_SIZE];
	int id;

	assert( msg != NULL );

	sscanf(msg, "%s %d",
		cmd, &id);

	delMsgInCheckFront(client->listSendMsg, id);
}

void proto_send_init_server(int type, client_t *client, client_t *client2)
{
	char msg[STR_PROTO_SIZE];
	int count;
	int check_id;

	assert( client2 != NULL );

	count = WORLD_COUNT_ROUND_UNLIMITED;

#ifndef PUBLIC_SERVER
	getSettingCountRound(&count);
#endif

	check_id = getNewID();
	
	sprintf(msg, "init %d %d %d %d %s %d\n",
		client2->tux->id, client2->tux->x, client2->tux->y,
		count, getArenaNetName( getChoiceArenaId() ), check_id);
	
	//proto_send(type, client, msg);
	protoSendClient(type, client, msg, CHECK_FORNT_TYPE_CHECK, check_id);
}

#ifndef PUBLIC_SERVER

void proto_recv_init_client(char *msg)
{
	char cmd[STR_PROTO_SIZE];
	char arena_name[STR_PROTO_SIZE];
	tux_t *tux;
	int id;
	int x, y, n;
	int check_id;

	assert( msg != NULL );

	sscanf(msg, "%s %d %d %d %d %s %d",
	cmd, &id, &x, &y, &n, arena_name, &check_id);

	proto_send_check_client(check_id);
	
	if( getCurrentArena() != NULL )
	{
		return;
	}

	setWorldArena( getArenaIdFormNetName(arena_name) );
	setMaxCountRound(n);

	tux = newTux();
	replaceTuxID(tux, id);
	tux->x = x;
	tux->y = y;
	tux->control = TUX_CONTROL_KEYBOARD_RIGHT;
	setControlTux(tux, TUX_CONTROL_KEYBOARD_RIGHT);

	getSettingNameRight(tux->name);

	addObjectToSpace(getCurrentArena()->spaceTux, tux);
}

#endif

void proto_send_event_server(int type, client_t *client, tux_t *tux, int action)
{
	char msg[STR_PROTO_SIZE];
	
	sprintf(msg, "event %d %d\n", tux->id, action);
	
	//proto_send(type, client, msg);
	protoSendClient(type, client, msg, CHECK_FORNT_TYPE_SIMPLE, CHECK_FRONT_ID_NONE);
}

#ifndef PUBLIC_SERVER

void proto_recv_event_client(char *msg)
{
	char cmd[STR_PROTO_SIZE];
	int id, action;
	tux_t *tux;

	assert( msg != NULL );

	sscanf(msg, "%s %d %d", cmd, &id, &action);

	tux = getObjectFromSpaceWithID(getCurrentArena()->spaceTux, id);

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

	//debug_interval();

	sscanf(msg, "%s %d", cmd, &action);

	if( client->tux->bonus != BONUS_HIDDEN )
	{
		proto_send_event_server(PROTO_SEND_ALL_SEES_TUX, client, client->tux, action);
	}

	refreshLastMove(client->protect);

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

	getTuxProportion(tux, &x, &y, NULL, NULL);

	sprintf(msg, "newtux %d %d %d %d %d %d %d %s %d %d %d %d %d %d %d %d %d %d %d\n",
	tux->id ,x, y, tux->status, tux->position ,tux->frame, tux->score, tux->name,
	tux->gun, tux->bonus, tux->shot[GUN_SIMPLE], tux->shot[GUN_DUAL_SIMPLE],
	tux->shot[GUN_SCATTER], tux->shot[GUN_TOMMY], tux->shot[GUN_LASSER],
	tux->shot[GUN_MINE], tux->shot[GUN_BOMBBALL],
	tux->bonus_time, tux->pickup_time);
	
	//proto_send(type, client, msg);
	protoSendClient(type, client, msg, CHECK_FORNT_TYPE_SIMPLE, CHECK_FRONT_ID_NONE);
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

	tux = getObjectFromSpaceWithID(getCurrentArena()->spaceTux, id);

	if( tux != NULL )
	{
		assert( tux->id == id );
	}

	if( tux == NULL )
	{
		char term_msg[STR_SIZE];
		sprintf(term_msg, "connect new client( id = %d)\n", id);
		appendTextInTerm(term_msg);

		tux = newTux();
		replaceTuxID(tux, id);
		tux->control = TUX_CONTROL_NET;
		addObjectToSpace(getCurrentArena()->spaceTux, tux);
	}

	moveObjectInSpace(getCurrentArena()->spaceTux, tux, x, y);
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
	
	//proto_send(type, client, msg);
	protoSendClient(type, client, msg, CHECK_FORNT_TYPE_SIMPLE, CHECK_FRONT_ID_NONE);
}

#ifndef PUBLIC_SERVER

void proto_recv_kill_client(char *msg)
{
	char cmd[STR_PROTO_SIZE];
	int id;
	tux_t *tux;

	assert( msg != NULL );

	sscanf(msg, "%s %d", cmd, &id);

	tux = getObjectFromSpaceWithID(getCurrentArena()->spaceTux, id);

	if( tux != NULL )
	{
		eventTuxIsDead(tux);
	}
}

#endif

void proto_send_del_server(int type, client_t *client, int id)
{
	char msg[STR_PROTO_SIZE];
	int check_id;

	check_id = getNewID();

	sprintf(msg, "del %d %d\n",
		id, check_id);

	protoSendClient(type, client, msg, CHECK_FORNT_TYPE_CHECK, check_id);
	//proto_check(type, client, msg, check_id);
}

#ifndef PUBLIC_SERVER

void proto_recv_del_client(char *msg)
{
	char cmd[STR_PROTO_SIZE];
	int id, check_id;
	tux_t *tux;
	shot_t *shot;
	item_t *item;

	assert( msg != NULL );

	sscanf(msg, "%s %d %d",
		cmd, &id, &check_id);

	proto_send_check_client(check_id);

	tux = getObjectFromSpaceWithID(getCurrentArena()->spaceTux, id);

	if( tux != NULL )
	{
		char term_msg[STR_SIZE];
	
		//printf("delete tux..\n");
		sprintf(term_msg, "tux with id %d is disconnect\n", tux->id);
		appendTextInTerm(term_msg);

		delObjectFromSpaceWithObject(getCurrentArena()->spaceTux, tux, destroyTux);
		return;
	}

	shot = getObjectFromSpaceWithID(getCurrentArena()->spaceShot, id);

	if( shot != NULL )
	{
		//printf("delete shot..\n");
		delObjectFromSpaceWithObject(getCurrentArena()->spaceShot, shot, destroyShot);
		return;
	}

	item = getObjectFromSpaceWithID(getCurrentArena()->spaceItem, id);

	if( item != NULL )
	{
		//printf("delete item..\n");
		delObjectFromSpaceWithObject(getCurrentArena()->spaceItem, item, destroyItem);
		return;
	}
}

#endif

void proto_send_additem_server(int type, client_t *client, item_t *p)
{
	char msg[STR_PROTO_SIZE];
	int check_id;

	assert( p != NULL );

	check_id = getNewID();

	sprintf(msg, "additem %d %d %d %d %d %d %d %d\n",
		p->id, p->type, p->x, p->y, p->count, p->frame, p->author_id, check_id);

	//proto_send(type, client, msg);
	//proto_check(type, client, msg, check_id);
	protoSendClient(type, client, msg, CHECK_FORNT_TYPE_CHECK, check_id);
}

#ifndef PUBLIC_SERVER

void proto_recv_additem_client(char *msg)
{
	char cmd[STR_PROTO_SIZE];
	int id, type, x, y, count, frame, author_id, check_id;
	item_t *item;

	assert( msg != NULL );

	if( getCurrentArena() == NULL )
	{
		return;
	}

	sscanf(msg, "%s %d %d %d %d %d %d %d %d",
		cmd, &id, &type, &x, &y, &count, &frame, &author_id, &check_id);

	proto_send_check_client(check_id);

	if( ( item = getObjectFromSpaceWithID(getCurrentArena()->spaceItem, id) ) != NULL )
	{
		return;
	}

	item = newItem(x, y, type, author_id);

	replaceItemID(item, id);
	item->count = count;
	item->frame = frame;

	addObjectToSpace(getCurrentArena()->spaceItem, item);
}

#endif

void proto_send_shot_server(int type, client_t *client, shot_t *p)
{
	char msg[STR_PROTO_SIZE];

	assert( p != NULL );

	sprintf(msg, "shot %d %d %d %d %d %d %d %d %d\n",
		p->id, p->x, p->y, p->px, p->py, p->position, p->gun, p->author_id, p->isCanKillAuthor);

	//proto_send(type, client, msg);
	//proto_check(type, client, msg, check_id);
	protoSendClient(type, client, msg, CHECK_FORNT_TYPE_SIMPLE, CHECK_FRONT_ID_NONE);
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


	if( ( shot = getObjectFromSpaceWithID(getCurrentArena()->spaceShot, shot_id) )  != NULL )
	{
		delObjectFromSpaceWithObject(getCurrentArena()->spaceShot, shot, destroyShot);

		//return;
	}

	shot = newShot(x, y, px, py, gun, author_id);

	replaceShotID(shot, shot_id);
	shot->isCanKillAuthor = isCanKillAuthor;
	shot->position = position;

	if( shot->gun == GUN_LASSER )
	{
		transformOnlyLasser(shot);
	}

	addObjectToSpace(getCurrentArena()->spaceShot, shot);
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
	//proto_send(type, client, msg);
	protoSendClient(type, client, msg, CHECK_FORNT_TYPE_SIMPLE, CHECK_FRONT_ID_NONE);
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
	//proto_send(type, client, msg);
	protoSendClient(type, client, msg, CHECK_FORNT_TYPE_SIMPLE, CHECK_FRONT_ID_NONE);
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

