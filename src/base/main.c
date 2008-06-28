
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <assert.h>
#include <sys/stat.h>

#include "base/main.h"
#include "base/tux.h"

#ifndef PUBLIC_SERVER
#include "client/game.h"
#endif

#ifdef PUBLIC_SERVER
#include "server/publicServer.h"
#endif

static int my_argc;
static char **my_argv;

list_t *listHelp;

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

char *getString(int n)
{
	char str[STR_NUM_SIZE];
	sprintf(str, "%d", n);
	return strdup(str);
}

int* newInt(int x)
{
	int *new;
	new = malloc( sizeof(int) );
	*new = x;
	return new;
}

void accessExistFile(const char *s)
{
	if( access(s, F_OK) != 0 )
	{
		fprintf(stderr, "File %s not found !\nProgram shutdown !\n", s);
		exit(-1);
	}
}

int main(int argc, char *argv[])
{
	listHelp = newList();

	srand( (unsigned) time(NULL) );

	my_argc = argc;
	my_argv = argv;
/*
	test_space();
	exit(0);
*/

#ifndef PUBLIC_SERVER
	startGame();
#endif

#ifdef PUBLIC_SERVER
	startPublicServer();
#endif

	return 0;
}
