
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

typedef struct struct_checkfront_t
{
	char *msg;
	my_time_t time;
	int id;
	int count;
} checkfront_t;

static checkfront_t* newCheck(char *s, int id)
{
	checkfront_t *new;

	assert( s != NULL );

	new = malloc( sizeof(checkfront_t) );
	memset(new, 0, sizeof(checkfront_t));

	new->msg = strdup(s);
	new->time = getMyTime();
	new->id = id;
	new->count = 0;

	return new;
}

static void destroyCheck(checkfront_t *p)
{
	assert( p != NULL );
	
	free(p->msg);
	free(p);
}


list_t* newCheckFront()
{
	return newList();
}

void addMsgInCheckFront(list_t *list, char *msg, int id)
{
	addList(list, newCheck(msg, id) );
}


void eventMsgInCheckFront(client_t *client)
{
	my_time_t currentTime;
	int i;

	currentTime = getMyTime();

	for( i = 0 ; i < client->listCheck->count ; i++ )
	{
		checkfront_t *this;

		this = (checkfront_t *)client->listCheck->list[i];

		if( this->count == 0 || currentTime - this->time > CHECK_FRONT_SEND_TIME_INTERVAL )
		{
			sendClient(client, this->msg);
			this->time = currentTime;
			this->count++;
		}

		if( this->count > CHECK_FRONT_MAX_COUNT_SEND )
		{
			if( isRegisterID(this->id) != -1 )
			{
				delID(this->id);
			}

			delListItem(client->listCheck, i, destroyCheck);
			i--;
		}
	}
}

void delMsgInCheckFront(list_t *listCheckFront, int id)
{
	int i;

	for( i = 0 ; i < listCheckFront->count ; i++ )
	{
		checkfront_t *this;

		this = (checkfront_t *)listCheckFront->list[i];

		if( this->id == id )
		{
			if( isRegisterID(this->id) != -1 )
			{
				delID(this->id);
			}

			delListItem(listCheckFront, i, destroyCheck);
			return;
		}
	}
}

void destroyCheckFront(list_t *p)
{
	assert( p != NULL );

	destroyListItem(p, destroyCheck);
}

