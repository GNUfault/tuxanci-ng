
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "main.h"
#include "list.h"
#include "myTimer.h"
#include "checkFront.h"
#include "idManager.h"
#include "server.h"

typedef struct struct_checkfront_t {
	char *msg;
	my_time_t time;
	int id;
	int count;
	int type;
} checkfront_t;

static checkfront_t *newCheck(char *s, int type, int id)
{
	checkfront_t *new;

	assert(s != NULL);

	new = malloc(sizeof(checkfront_t));
	memset(new, 0, sizeof(checkfront_t));
	new->msg = strdup(s);
	new->time = timer_get_current_time();
	new->id = id;
	new->type = type;
	new->count = 0;

	return new;
}

static void destroyCheck(checkfront_t * p)
{
	assert(p != NULL);
	free(p->msg);
	free(p);
}

list_t *check_front_new()
{
	return list_new();
}

void check_front_msg_add(list_t * list, char *msg, int type, int id)
{
	if (type == CHECK_FRONT_TYPE_CHECK)
		id_inc(id);

	list_add(list, newCheck(msg, type, id));
}

void check_front_event(client_t * client)
{
	my_time_t currentTime;
	int i;

	currentTime = timer_get_current_time();

	for (i = 0; i < client->listSendMsg->count; i++) {
		checkfront_t *this;

		this = (checkfront_t *) client->listSendMsg->list[i];

		switch (this->type) {
			case CHECK_FRONT_TYPE_SIMPLE:
				server_send_client(client, this->msg);
				list_del_item(client->listSendMsg, i, destroyCheck);
				i--;
				break;
			case CHECK_FRONT_TYPE_CHECK:
				if (this->count == 0 || currentTime - this->time > CHECK_FRONT_SEND_TIME_INTERVAL) {
					server_send_client(client, this->msg);
					this->time = currentTime;
					this->count++;
				}
				if (this->count > CHECK_FRONT_MAX_COUNT_SEND) {
					id_del(this->id);
					list_del_item(client->listSendMsg, i, destroyCheck);
					i--;
				}
				break;
			default:
				assert(!_("Bad type of front!"));
				break;
		}
	}
}

void check_front_del_msg(list_t * listCheckFront, int id)
{
	int i;

	for (i = 0; i < listCheckFront->count; i++) {
		checkfront_t *this;

		this = (checkfront_t *) listCheckFront->list[i];

		if (this->id == id) {
			id_del(this->id);
			list_del_item(listCheckFront, i, destroyCheck);
			return;
		}
	}
}

void check_front_destroy(list_t * p)
{
	assert(p != NULL);
	list_destroy_item(p, destroyCheck);
}
