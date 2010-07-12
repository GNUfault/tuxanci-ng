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
#include "checkFront.h"
#include "idManager.h"
#include "modules.h"
#include "protect.h"
#include "serverSendMsg.h"

#ifndef PUBLIC_SERVER
#include "world.h"
#include "setting.h"
#include "choiceArena.h"
#include "analyze.h"
#include "downArena.h"
#include "client.h"
#include "term.h"
#include "radar.h"
#include "chat.h"
#else /* PUBLIC_SERVER*/
#include "highScore.h"
#include "publicServer.h"
#include "serverConfigFile.h"
#endif /* PUBLIC_SERVER*/

void proto_send_error_server(int type, client_t *client, int errorcode)
{
	char msg[STR_PROTO_SIZE];

	snprintf(msg, STR_PROTO_SIZE, "error %d\n", errorcode);

	send_msg_to_client(type, client, msg, CHECK_FRONT_TYPE_SIMPLE, CHECK_FRONT_ID_NONE);
}

#ifndef PUBLIC_SERVER
void proto_recv_error_client(char *msg)
{
	char cmd[STR_PROTO_SIZE];
	int errorcode;
	int recv_count;

	assert(msg != NULL);

	recv_count = sscanf(msg, "%s %d", cmd, &errorcode);

	if (recv_count != 2) {
		return;
	}

	debug("Communication error [%d]", errorcode);

	switch (errorcode) {
		case PROTO_ERROR_CODE_BAD_VERSION:
			analyze_set_msg(_("Cannot connect to the server. You have incompatible version of the game."));
			world_do_end();
			break;
		case PROTO_ERROR_CODE_TIMEOUT:
			analyze_set_msg(_("Connection timeout"));
			world_do_end();
			break;
		case PROTO_ERROR_LIMIT_MAX_CLIENT:
			analyze_set_msg(_("Server is full"));
			world_do_end();
			break;
	}
}

void proto_send_hello_client(char *name)
{
	char msg[STR_PROTO_SIZE];

	snprintf(msg, STR_PROTO_SIZE, "hello %s %s\n",
		 getParamElse("--version", TUXANCI_VERSION), name);

	debug("Sending: %s", msg);
	client_send(msg);
}
#endif /* PUBLIC_SERVER */

static void action_sendItem(space_t *space, item_t *item, client_t *client)
{
	proto_send_additem_server(PROTO_SEND_ONE, client, item);
}

static void sendInfoCreateClient(client_t *client)
{
	list_t *listClient;
	client_t *thisClient;
	tux_t *thisTux;
	int i;

	assert(client != NULL);

	listClient = server_get_list_clients();

	proto_send_init_server(PROTO_SEND_ONE, client, client);

#ifndef PUBLIC_SERVER
	proto_send_newtux_server(PROTO_SEND_ONE, client, world_get_control_tux(TUX_CONTROL_KEYBOARD_RIGHT));
#endif /* PUBLIC_SERVER */

	for (i = 0; i < listClient->count; i++) {
		thisClient = (client_t *) listClient->list[i];
		thisTux = thisClient->tux;

		/* odosli kazdemu stremu hracovy informacie o novom hracovy */
		/* send to all players information about new player */
		if (thisTux != NULL && thisTux != client->tux) { 
			proto_send_newtux_server(PROTO_SEND_ONE, thisClient, client->tux);
		}

		/* send to new players information about all players, including himself */
		if (thisTux != NULL) {
			proto_send_newtux_server(PROTO_SEND_ONE, client, thisTux);
		}
	}

	space_action(arena_get_current()->spaceItem, action_sendItem, client);

/*
	for (i = 0; i < arena_get_current()->spaceItem->listIndex->count; i++) {
		thisItem = (item_t *) arena_get_current()->spaceItem->list->list[i];
		proto_send_additem_server(PROTO_SEND_ONE, client, thisItem);
	}
*/
}

void proto_recv_hello_server(client_t *client, char *msg)
{
	static char *supportVersion = NULL;
	char cmd[STR_PROTO_SIZE];
	char version[STR_NAME_SIZE];
	char name[STR_NAME_SIZE];
	int recv_count;

	assert(client != NULL);

	if (space_get_count(arena_get_current()->spaceTux) + 1 > server_get_max_clients()) {
		proto_send_error_server(PROTO_SEND_ONE, client, PROTO_ERROR_LIMIT_MAX_CLIENT);
		check_front_event(client);
		client->status = NET_STATUS_ZOMBIE;
		return;
	}

	strcpy(version, "");
	strcpy(name, "");

	if (supportVersion == NULL) {
#ifndef PUBLIC_SERVER
		supportVersion = TUXANCI_VERSION;
#else /* PUBLIC_SERVER */
		supportVersion = public_server_get_setting("SUPPORT_CLIENTS", "--support-clients", TUXANCI_VERSION);
#endif /* PUBLIC_SERVER */
	}

	recv_count = sscanf(msg, "%s %s %s", cmd, version, name);

	if (recv_count != 3) {
		return;
	}

	if (strstr(supportVersion, version) == NULL) {
		proto_send_error_server(PROTO_SEND_ONE, client, PROTO_ERROR_CODE_BAD_VERSION);
		check_front_event(client);
		client->status = NET_STATUS_ZOMBIE;
		return;
	}

	client->tux = tux_new();
	client->tux->control = TUX_CONTROL_NET;
	client->tux->client = client;

	space_add(arena_get_current()->spaceTux, client->tux);

	if (strlen(name) > STR_NAME_SIZE - 1) {
		name[STR_NAME_SIZE - 1] = '\0';
	}

	tux_set_name(client->tux, name);
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
	name = "NewServer";
#endif /* PUBLIC_SERVER */

#ifdef PUBLIC_SERVER
	name = public_server_get_setting("NAME", "--name", "noname");
#endif /* PUBLIC_SERVER */

	version = TUXANCI_VERSION;
	clients = arena_get_current()->spaceTux->listIndex->count;
	maxclients = server_get_max_clients();
	uptime = (unsigned int) server_get_update();
	arena = arena_file_get_net_name(choice_arena_get());

	snprintf(msg, STR_PROTO_SIZE,
		 "name: %s\n"
		 "version: %s\n"
		 "clients: %d\n"
		 "maxclients: %d\n"
		 "uptime: %d\n"
		 "arena: %s\n",
		 name, version, clients, maxclients, uptime, arena);

	send_msg_to_client(type, client, msg, CHECK_FRONT_TYPE_SIMPLE, CHECK_FRONT_ID_NONE);
	/*proto_send(type, client, msg);*/
}

void proto_recv_status_server(client_t *client, char *msg)
{
	proto_send_status_server(PROTO_SEND_ONE, client);
}

#ifndef PUBLIC_SERVER
void proto_send_check_client(int id)
{
	char msg[STR_PROTO_SIZE];

	snprintf(msg, STR_PROTO_SIZE, "check %d\n", id);
	client_send(msg);
}
#else /* PUBLIC_SERVER */
void proto_send_listscore_server(int type, client_t *client, int max)
{
	char *str;
	char *msg;
	int i;

	msg = malloc((max * 48) * sizeof(char));
	strcpy(msg, "");

	for (i = 0; i < max; i++) {
		str = high_score_get_table(i);

		if (str == NULL) {
			break;
		}

		strcat(msg, str);
		strcat(msg, "\n");
	}

	/*proto_send(type, client, msg);*/
	send_msg_to_client(type, client, msg, CHECK_FRONT_TYPE_SIMPLE, CHECK_FRONT_ID_NONE);
	free(msg);
}

void proto_recv_listscore_server(client_t *client, char *msg)
{
	char cmd[STR_PROTO_SIZE];
	int recv_count;
	int max;

	assert(msg != NULL);

	recv_count = sscanf(msg, "%s %d", cmd, &max);

	if (recv_count != 2) {
		max = HIGHSCORE_MAX_PLAYERS;
	}

	proto_send_listscore_server(PROTO_SEND_ONE, client, max);
}
#endif /* PUBLIC_SERVER */

void proto_recv_check_server(client_t *client, char *msg)
{
	char cmd[STR_PROTO_SIZE];
	int recv_count;
	int id;

	assert(msg != NULL);

	recv_count = sscanf(msg, "%s %d", cmd, &id);

	if (recv_count != 2) {
		return;
	}

	check_front_del_msg(client->listSendMsg, id);
}

void proto_send_init_server(int type, client_t *client, client_t *client2)
{
	char msg[STR_PROTO_SIZE];
	int count;
	int check_id;

	assert(client2 != NULL);

	count = WORLD_COUNT_ROUND_UNLIMITED;

#ifndef PUBLIC_SERVER
	public_server_get_settingCountRound(&count);
#endif /* PUBLIC_SERVER */

	check_id = id_get_newcount(0);

	snprintf(msg, STR_PROTO_SIZE, "init %d %d %d %d %s %d\n",
		 client2->tux->id, client2->tux->x, client2->tux->y,
		 count, arena_file_get_net_name(choice_arena_get()), check_id);

	/*proto_send(type, client, msg);*/
	send_msg_to_client(type, client, msg, CHECK_FRONT_TYPE_CHECK, check_id);
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
	int recv_count;
	arena_t *arena;

	assert(msg != NULL);

	recv_count = sscanf(msg, "%s %d %d %d %d %s %d",
			    cmd, &id, &x, &y, &n, arena_name, &check_id);

	if (recv_count != 7) {
		return;
	}

	proto_send_check_client(check_id);

	if (arena_get_current() != NULL) {
		return;
	}

	world_set_arena(arena_file_get_file_format_net_name(arena_name));

	arena = arena_get_current();
	arena->max_countRound = n;

	tux = tux_new();
	tux_replace_id(tux, id);
	tux->x = x;
	tux->y = y;
	tux->control = TUX_CONTROL_KEYBOARD_RIGHT;
	world_set_control_tux(tux, TUX_CONTROL_KEYBOARD_RIGHT);

	public_server_get_setting_name_right(tux->name);

	space_add(arena->spaceTux, tux);
}
#endif /* PUBLIC_SERVER */

void proto_send_event_server(int type, client_t *client, tux_t *tux, int action)
{
	char msg[STR_PROTO_SIZE];

	snprintf(msg, STR_PROTO_SIZE, "event %d %d\n", tux->id, action);

	/*proto_send(type, client, msg);*/
	send_msg_to_client(type, client, msg, CHECK_FRONT_TYPE_SIMPLE, CHECK_FRONT_ID_NONE);
}

#ifndef PUBLIC_SERVER
void proto_recv_event_client(char *msg)
{
	char cmd[STR_PROTO_SIZE];
	int id, action;
	tux_t *tux;
	int recv_count;

	assert(msg != NULL);

	recv_count = sscanf(msg, "%s %d %d", cmd, &id, &action);

	if (recv_count != 3) {
		return;
	}

	tux = space_get_object_id(arena_get_current()->spaceTux, id);

	if (tux != NULL) {
		tux_action(tux, action);
		radar_add(tux->id, tux->x, tux->y, RADAR_TYPE_TUX);
	}
}

void proto_send_event_client(int action)
{
	char msg[STR_PROTO_SIZE];

	snprintf(msg, STR_PROTO_SIZE, "event %d\n", action);

	client_send(msg);
}
#endif /* PUBLIC_SERVER */

void proto_recv_event_server(client_t *client, char *msg)
{
	char cmd[STR_PROTO_SIZE];
	int action;
	int recv_count;

	assert(client != NULL);
	assert(msg != NULL);

	/*debug_interval();*/

	recv_count = sscanf(msg, "%s %d", cmd, &action);

	if (recv_count != 2) {
		return;
	}

	if (client->tux->bonus != BONUS_HIDDEN) {
		proto_send_event_server(PROTO_SEND_ALL_SEES_TUX, client, client->tux, action);
	}

	refreshLastMove(client->protect);

	tux_action(client->tux, action);
}

void proto_send_newtux_server(int type, client_t *client, tux_t *tux)
{
	char msg[STR_PROTO_SIZE];
	int x, y;

	assert(tux != NULL);

	if (tux->bonus == BONUS_HIDDEN) {
		x = TUX_LOCATE_UNKNOWN;
		y = TUX_LOCATE_UNKNOWN;
	} else {
		tux_get_proportion(tux, &x, &y, NULL, NULL);
	}

	snprintf(msg, STR_PROTO_SIZE, "newtux %d %d %d %d %d %d %d %s %d %d %d %d %d %d %d %d %d %d %d\n",
		 tux->id, x, y, tux->status, tux->position, tux->frame,
		 tux->score, tux->name, tux->gun, tux->bonus,
		 tux->shot[GUN_SIMPLE], tux->shot[GUN_DUAL_SIMPLE],
		 tux->shot[GUN_SCATTER], tux->shot[GUN_TOMMY],
		 tux->shot[GUN_LASSER], tux->shot[GUN_MINE],
		 tux->shot[GUN_BOMBBALL], tux->bonus_time, tux->pickup_time);

	/*proto_send(type, client, msg);*/
	send_msg_to_client(type, client, msg, CHECK_FRONT_TYPE_SIMPLE,
					      CHECK_FRONT_ID_NONE);
}

#ifndef PUBLIC_SERVER
void proto_recv_newtux_client(char *msg)
{
	char cmd[STR_PROTO_SIZE];
	char name[STR_NAME_SIZE];
	int id, x, y, status, position, frame, score;
	int myGun, myBonus;
	int gun1, gun2, gun3, gun4, gun5, gun6, gun7;
	int time1, time2;
	tux_t *tux;
	int recv_count;

	assert(msg != NULL);

	if (arena_get_current() == NULL) {
		return;
	}

	recv_count = sscanf(msg, "%s %d %d %d %d %d %d %d %s %d %d %d %d %d %d %d %d %d %d %d",
			    cmd, &id, &x, &y, &status, &position, &frame, &score, name,
			    &myGun, &myBonus, &gun1, &gun2, &gun3, &gun4, &gun5, &gun6,
			    &gun7, &time1, &time2);

	if (recv_count != 20) {
		return;
	}

	tux = space_get_object_id(arena_get_current()->spaceTux, id);

	if (tux != NULL) {
		assert(tux->id == id);
	}

	if (tux == NULL) {
		tux = tux_new();
		tux_replace_id(tux, id);
		tux->control = TUX_CONTROL_NET;
		space_add(arena_get_current()->spaceTux, tux);
	}

	if (tux->control == TUX_CONTROL_KEYBOARD_RIGHT &&
	    x == TUX_LOCATE_UNKNOWN && y == TUX_LOCATE_UNKNOWN) {
		tux_get_proportion(tux, &x, &y, NULL, NULL);
	}

	radar_add(id, x, y, RADAR_TYPE_TUX);

	if (tux->bonus == BONUS_HIDDEN) {
		radar_del(id);
	}

	space_move_object(arena_get_current()->spaceTux, tux, x, y);
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
#endif /* PUBLIC_SERVER */

void proto_send_kill_server(int type, client_t *client, tux_t *tux)
{
	char msg[STR_PROTO_SIZE];

	assert(tux != NULL);

	snprintf(msg, STR_PROTO_SIZE, "kill %d\n", tux->id);

	/*proto_send(type, client, msg);*/
	send_msg_to_client(type, client, msg, CHECK_FRONT_TYPE_SIMPLE, CHECK_FRONT_ID_NONE);
}

#ifndef PUBLIC_SERVER
void proto_recv_kill_client(char *msg)
{
	char cmd[STR_PROTO_SIZE];
	int id;
	tux_t *tux;
	int recv_count;

	assert(msg != NULL);

	recv_count = sscanf(msg, "%s %d", cmd, &id);

	if (recv_count != 2) {
		return;
	}

	tux = space_get_object_id(arena_get_current()->spaceTux, id);

	if (tux != NULL) {
		tux_event_tux_is_dead(tux);
	}
}
#endif /* PUBLIC_SERVER */

void proto_send_del_server(int type, client_t *client, int id)
{
	char msg[STR_PROTO_SIZE];
	int check_id;

	check_id = id_get_newcount(0);

	snprintf(msg, STR_PROTO_SIZE, "del %d %d\n", id, check_id);

	send_msg_to_client(type, client, msg, CHECK_FRONT_TYPE_CHECK, check_id);
	/*proto_check(type, client, msg, check_id);*/
}

#ifndef PUBLIC_SERVER

void timer_delShot(void *p)
{
	shot_t *shot;
	int *id;

	assert( p != NULL );
	id = (int *) p;

	shot = space_get_object_id(arena_get_current()->spaceShot, *id);

	if (shot != NULL) {
		radar_del(shot->id);
		space_del_with_item(arena_get_current()->spaceShot, shot, shot_destroy);
	}

	free(p);
}

void proto_recv_del_client(char *msg)
{
	char cmd[STR_PROTO_SIZE];
	int id, check_id;
	tux_t *tux;
	shot_t *shot;
	item_t *item;
	int recv_count;

	assert(msg != NULL);

	recv_count = sscanf(msg, "%s %d %d", cmd, &id, &check_id);

	if (recv_count != 3) {
		return;
	}

	proto_send_check_client(check_id);

	tux = space_get_object_id(arena_get_current()->spaceTux, id);

	if (tux != NULL) {
		radar_del(id);
		space_del_with_item(arena_get_current()->spaceTux, tux, tux_destroy);
		return;
	}

	shot = space_get_object_id(arena_get_current()->spaceShot, id);

	if (shot != NULL) {
		/*
		printf("delete shot..\n");
		radar_del(id);
		space_del_with_item(arena_get_current()->spaceShot, shot, shot_destroy);
		*/
		timer_add_task(arena_get_current()->listTimer, TIMER_ONE, timer_delShot, newInt(id), 50);
		return;
	}

	item = space_get_object_id(arena_get_current()->spaceItem, id);

	if (item != NULL) {
		/*printf("delete item..\n");*/
		radar_del(id);
		space_del_with_item(arena_get_current()->spaceItem, item, item_destroy);
		return;
	}
}
#endif /* PUBLIC_SERVER */

void proto_send_additem_server(int type, client_t *client, item_t *p)
{
	char msg[STR_PROTO_SIZE];
	int check_id;

	assert(p != NULL);

	check_id = id_get_newcount(0);

	snprintf(msg, STR_PROTO_SIZE, "additem %d %d %d %d %d %d %d %d\n",
		 p->id, p->type, p->x, p->y, p->count, p->frame, p->author_id, check_id);

	/*
	proto_send(type, client, msg);
	proto_check(type, client, msg, check_id);
	*/
	send_msg_to_client(type, client, msg, CHECK_FRONT_TYPE_CHECK, check_id);
}

#ifndef PUBLIC_SERVER
void proto_recv_additem_client(char *msg)
{
	char cmd[STR_PROTO_SIZE];
	int id, type, x, y, count, frame, author_id, check_id;
	item_t *item;
	int recv_count;

	assert(msg != NULL);

	if (arena_get_current() == NULL) {
		return;
	}

	recv_count = sscanf(msg, "%s %d %d %d %d %d %d %d %d",
			    cmd, &id, &type, &x, &y, &count, &frame, &author_id, &check_id);

	if (recv_count != 9) {
		return;
	}

	proto_send_check_client(check_id);

	if ((item = space_get_object_id(arena_get_current()->spaceItem, id)) != NULL) {
		return;
	}

	if (type != ITEM_MINE && type != ITEM_EXPLOSION &&
	    type != ITEM_BIG_EXPLOSION) {
		radar_add(id, x, y, RADAR_TYPE_ITEM);
	}

	item = item_new(x, y, type, author_id);

	item_replace_id(item, id);
	item->count = count;
	item->frame = frame;

	space_add(arena_get_current()->spaceItem, item);
}
#endif /* PUBLIC_SERVER */

void proto_send_shot_server(int type, client_t *client, shot_t *p)
{
	char msg[STR_PROTO_SIZE];

	assert(p != NULL);

	snprintf(msg, STR_PROTO_SIZE, "shot %d %d %d %d %d %d %d %d %d\n",
//	 	 p->id, p->x-p->px*8, p->y-p->py*8, p->px, p->py, p->position, p->gun,
	 	 p->id, p->x, p->y, p->px, p->py, p->position, p->gun,
		 p->author_id, p->isCanKillAuthor);

	/*
	proto_send(type, client, msg);
	proto_check(type, client, msg, check_id);
	*/
	send_msg_to_client(type, client, msg, CHECK_FRONT_TYPE_SIMPLE, CHECK_FRONT_ID_NONE);
}

#ifndef PUBLIC_SERVER
void proto_recv_shot_client(char *msg)
{
	char cmd[STR_PROTO_SIZE];
	int x, y, px, py, position, gun, shot_id, author_id, isCanKillAuthor;
	shot_t *shot;
	int recv_count;

	assert(msg != NULL);

	recv_count = sscanf(msg, "%s %d %d %d %d %d %d %d %d %d",
			    cmd, &shot_id, &x, &y, &px, &py, &position, &gun,
			    &author_id, &isCanKillAuthor);

	if (recv_count != 10) {
		return;
	}

	if ((shot = space_get_object_id(arena_get_current()->spaceShot, shot_id)) != NULL) {
		space_del_with_item(arena_get_current()->spaceShot, shot, shot_destroy);
		/*return;*/
	}

	shot = shot_new(x, y, px, py, gun, author_id);

	shot_replace_id(shot, shot_id);
	shot->isCanKillAuthor = isCanKillAuthor;
	shot->position = position;

	if (shot->gun == GUN_LASSER) {
		shot_transform_lasser(shot);
	}

	space_add(arena_get_current()->spaceShot, shot);
}

void proto_send_chat_client(char *s)
{
	char msg[STR_PROTO_SIZE];
	snprintf(msg, STR_PROTO_SIZE, "chat %s\n", s);
	client_send(msg);
}
#endif /* PUBLIC_SERVER */

void proto_recv_chat_server(client_t *client, char *msg)
{
	char out[STR_PROTO_SIZE];
	int len;

	if (client->tux == NULL) {
		return;
	}

	len = strlen(msg);
	msg[len - 1] = '\0';

	snprintf(out, STR_PROTO_SIZE, "chat %s:%s\n", client->tux->name, msg + 5);

	proto_send_chat_server(PROTO_SEND_ALL, NULL, out);
}

void proto_send_chat_server(int type, client_t *client, char *msg)
{
	send_msg_to_client(type, client, msg, CHECK_FRONT_TYPE_SIMPLE, CHECK_FRONT_ID_NONE);

#ifndef PUBLIC_SERVER
	proto_recv_chat_client(msg);
#endif /* PUBLIC_SERVER */
}

#ifndef PUBLIC_SERVER
void proto_recv_chat_client(char *msg)
{
	int len;

	assert(msg != NULL);

	len = strlen(msg);
	msg[len - 1] = '\0';

	chat_add(msg + 5);
}

void proto_send_module_client(char *msg)
{
	char out[STR_PROTO_SIZE];

	snprintf(out, STR_PROTO_SIZE, "module %s\n", msg);

/*
	strcpy(out, "modules ");
	strcat(out, msg);
	strcat(out, "\n");
*/

	client_send(out);
}
#endif /* PUBLIC_SERVER */

void proto_recv_module_server(client_t *client, char *msg)
{
	module_recv_msg(msg + 7);
}

void proto_send_module_server(int type, client_t *client, char *msg)
{
	char out[STR_PROTO_SIZE];

	snprintf(out, STR_PROTO_SIZE, "module %s\n", msg);

/*
	strcpy(out, "modules ");
	strcat(out, msg);
	strcat(out, "\n");
*/

	send_msg_to_client(type, client, out, CHECK_FRONT_TYPE_SIMPLE, CHECK_FRONT_ID_NONE);
}

#ifndef PUBLIC_SERVER
void proto_recv_module_client(char *msg)
{
	module_recv_msg(msg + 7);
}

void proto_send_ping_client()
{
	char msg[STR_PROTO_SIZE];
	snprintf(msg, STR_PROTO_SIZE, "ping\n");
	client_send(msg);
}
#endif /* PUBLIC_SERVER */

void proto_recv_ping_server(client_t *client, char *msg)
{
}

void proto_send_ping_server(int type, client_t *client)
{
	char msg[STR_PROTO_SIZE];
	snprintf(msg, STR_PROTO_SIZE, "ping\n");
	/*proto_send(type, client, msg);*/
	send_msg_to_client(type, client, msg, CHECK_FRONT_TYPE_SIMPLE, CHECK_FRONT_ID_NONE);
}

#ifndef PUBLIC_SERVER
void proto_recv_ping_client(char *msg)
{
}
#endif /* PUBLIC_SERVER */

void proto_recv_echo_server(client_t *client, char *msg)
{
	proto_send_echo_server(PROTO_SEND_ONE, client, msg + 4);	/* msg + strlen("echo"); */
}

void proto_send_echo_server(int type, client_t *client, char *s)
{
	char msg[STR_PROTO_SIZE];

	snprintf(msg, STR_PROTO_SIZE, "echo%s", s);
	/*proto_send(type, client, msg);*/
	send_msg_to_client(type, client, msg, CHECK_FRONT_TYPE_SIMPLE, CHECK_FRONT_ID_NONE);
}

void proto_send_end_server(int type, client_t *client)
{
	char msg[STR_PROTO_SIZE];

	snprintf(msg, STR_PROTO_SIZE, "end\n");
	/*proto_send(type, client, msg);*/
	send_msg_to_client(type, client, msg, CHECK_FRONT_TYPE_SIMPLE, CHECK_FRONT_ID_NONE);
}

#ifndef PUBLIC_SERVER
void proto_recv_end_client(char *msg)
{
	assert(msg != NULL);

	world_do_end();
}

void proto_send_end_client()
{
	char msg[STR_PROTO_SIZE];

	snprintf(msg, STR_PROTO_SIZE, "end\n");
	client_send(msg);
}
#endif /* PUBLIC_SERVER */

void proto_recv_end_server(client_t *client, char *msg)
{
	assert(msg != NULL);
	assert(client != NULL);

	client->status = NET_STATUS_ZOMBIE;
}
