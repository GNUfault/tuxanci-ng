
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <assert.h>
#include <stdlib.h>
#include <sys/time.h>
#include <signal.h>

#include "list.h"
#include "udp.h"

#define STR_MSG_SIZE	512
#define STR_IP4_SIZE	20

#define LIMIT_REGISTER	"100"
#define LIMIT_TIME	"10"

static sock_udp_t *socketServer;
static sock_udp_t *socketClient;
static list_t *listClient;

static int limitRegisterGameServer;
static int limitTime;

typedef struct client_struct
{
	char *msg;
	char *ip;
	time_t time;
} client_t;

static int my_argc;
static char **my_argv;

char* getParam(char *s)
{
	int i;
	int len;

	len = strlen(s);

	for( i = 1 ; i < my_argc ; i++ )
	{
		//printf("%s %s\n", s, my_argv[i]);
		
		if( strlen(my_argv[i]) < len )
		{
			continue;
		}

		if( strncmp(s, my_argv[i], len) == 0 )
		{
			return strchr(my_argv[i], '=')+1;
		}
	}

	return NULL;
}

char* getParamElse(char *s1, char *s2)
{
	char *ret;
	ret = getParam(s1);

	if( ret == NULL)
	{
		return s2;
	}

	return ret;
}

bool_t isParamFlag(char *s)
{
	int i;

	for( i = 0 ; i < my_argc ; i++ )
	{
		if( strcmp(s, my_argv[i]) == 0 )
		{
			return TRUE;
		}
	}

	return FALSE;
}

client_t* newClient(char *str_ip, char *msg)
{
	client_t *new;

	new = malloc( sizeof(client_t) );
	memset(new, 0, sizeof(client_t));
	new->ip = strdup(str_ip);
	new->msg = strdup(msg);
	new->time = time(NULL);

	return new;
}

void replaceClient(client_t *p, char *msg)
{
	assert( p != NULL );

	free(p->msg);
	p->msg = strdup(msg);
	p->time = time(NULL);
}

void destroyClient(client_t *p)
{
	assert( p != NULL );

	free(p->ip);
	free(p->msg);
	free(p);
}

void cmd_register(sock_udp_t *client, char *msg)
{
	char str_ip[STR_IP4_SIZE];
	int len;
	int i;

	if( listClient->count > limitRegisterGameServer )
	{
		return;
	}

	getSockUdpIp(client, str_ip);

	len = strlen(msg);

	if( msg[len-1] == '\n' )
	{
		msg[len-1] = '\0';
		len--;
	}

	for( i = 0 ; i < len ; i++ )
	{
		if( msg[i] < ' ' || msg[i] > '~' )
		{
			msg[i] = '?';
		}
	}

	for( i = 0 ; i < listClient->count ; i++ )
	{
		client_t *client;

		client = (client_t *)listClient->list[i];
		
		if( strcmp(client->ip, str_ip) == 0 )
		{
			replaceClient(client, msg+9);
			return;
		}
	}

	addList(listClient, newClient(str_ip, msg+9) );
}

void cmd_list(sock_udp_t *socketClient, char *msg)
{
	time_t currnetTime;
	char *out_msg;
	int len_out_msg;
	int i;

	if( listClient->count == 0 )
	{
		writeUdpSocket(socketServer, socketClient, "<|>\n", 4);
		return;
	}

	currnetTime = time(NULL);

	len_out_msg = listClient->count * STR_MSG_SIZE;
	out_msg = malloc(len_out_msg);
	memset(out_msg, 0, len_out_msg);

	len_out_msg--;

	for( i = 0 ; i < listClient->count ; i++ )
	{
		client_t *client;
		char line[STR_MSG_SIZE*2];
		int len_new_line;

		client = (client_t *)listClient->list[i];
		
		if( currnetTime - client->time > limitTime )
		{
			delListItem(listClient, i, destroyClient);
			i--;
			continue;
		}

		sprintf(line, "ip=\"%s\" %s\n", client->ip, client->msg);
		len_new_line = strlen(line);

		if( len_out_msg > len_new_line )
		{
			strcat(out_msg, line);
			len_out_msg -= len_new_line;
		}
		else
		{
			break;
		}
	}

	if( listClient->count == 0 )
	{
		writeUdpSocket(socketServer, socketClient, "<|>\n", 4);
		free(out_msg);
		return;
	}

	writeUdpSocket(socketServer, socketClient, out_msg, strlen(out_msg) );
	free(out_msg);
}

void my_handler_quit(int n)
{
	printf("my_handler_quit\n");

	destroyListItem(listClient, destroyClient);
	closeUdpSocket(socketServer);
	destroySockUdp(socketClient);

	exit(0);
}

int main(int argc, char **argv)
{
	my_argc = argc;
	my_argv = argv;

	signal(SIGPIPE, SIG_IGN);
	signal(SIGINT, my_handler_quit);
	signal(SIGTERM, my_handler_quit);
	signal(SIGQUIT, my_handler_quit);

	listClient = newList();

	limitRegisterGameServer = atoi( getParamElse("--limitclient", LIMIT_REGISTER) );
	limitTime = atoi( getParamElse("--limittime", LIMIT_TIME) );

	socketServer = bindUdpSocket(
		getParamElse("--ip", "0.0.0.0"),
		atoi(getParamElse("--port", "8400")) );

	socketClient = newSockUdp();

	for(;;)
	{
		char msg[STR_MSG_SIZE];

		memset(msg, 0, STR_MSG_SIZE);
		readUdpSocket(socketServer, socketClient, msg, STR_MSG_SIZE);

		if( strncmp(msg, "register", 8) == 0 )
		{
			cmd_register(socketClient, msg);
		}

		if( strncmp(msg, "list", 4) == 0 )
		{
			cmd_list(socketClient, msg);
		}
	}

	return 0;
}
