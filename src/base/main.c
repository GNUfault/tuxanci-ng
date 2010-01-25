#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <assert.h>
#include <sys/stat.h>

#ifdef __WIN32__
#include <windows.h>
#include <wininet.h>
#endif /* __WIN32__ */

#include "main.h"
#include "tux.h"

#ifndef PUBLIC_SERVER
#include "game.h"
#else /* PUBLIC_SERVER */
#include "publicServer.h"
#endif /* PUBLIC_SERVER */

static int my_argc;
static char **my_argv;

char *getParam(char *s)
{
	unsigned int len;
	int i;

	len = strlen(s);

	for (i = 1; i < my_argc; i++) {
		/*printf("%s %s\n", s, my_argv[i]);*/

		if (strlen(my_argv[i]) < len) {
			continue;
		}

		if (strncmp(s, my_argv[i], len) == 0 && strchr(my_argv[i], '=') != NULL ) {
			return strchr(my_argv[i], '=') + 1;
		}
	}

	return NULL;
}

char *getParamElse(char *s1, char *s2)
{
	char *ret;
	ret = getParam(s1);

	if (ret == NULL) {
		return s2;
	}

	return ret;
}

bool_t isParamFlag(char *s)
{
	int i;

	for (i = 0; i < my_argc; i++) {
		if (strcmp(s, my_argv[i]) == 0) {
			return TRUE;
		}
	}

	return FALSE;
}

char* get_program_name()
{
	return my_argv[0];
}

char *getString(int n)
{
	char str[STR_NUM_SIZE];
	sprintf(str, "%d", n);
	return strdup(str);
}

char *get_string_static(int n)
{
	static char str[STR_SIZE];
	sprintf(str, "%d", n);

	return str;	/* !!! */
}

int *newInt(int x)
{
	int *new;

	new = malloc(sizeof(int));
	*new = x;

	return new;
}

void accessExistFile(const char *s)
{
	if (access(s, F_OK) != 0) {
		error("File not found [%s]", s);
		error("Shutting down the game");
		exit(-1);
	}
}

int tryExistFile(const char *s)
{
	if (access(s, F_OK) != 0) {
		return 1;
	} else {
		return 0;
	}
}

int isFillPath(const char *path)
{
	assert(path != NULL);

#ifndef __WIN32__
	if (path[0] == '/') {	/* for Unix-like systems ;) */
#else /* __WIN32__ */
	if (path[1] == ':') {	/* for Windows-like systems ;) */
#endif /* __WIN32__ */
		return 1;
	}

	return 0;
}

#ifdef __WIN32__
int WINAPI
WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#else /* __WIN32__ */
int main(int argc, char *argv[])
#endif /* __WIN32__ */
{
#ifndef __WIN32__
	signal(SIGPIPE, SIG_IGN);
#endif /* __WIN32__ */

	srand((unsigned) time(NULL));

#ifdef NLS
	setlocale(LC_ALL, "");
	bindtextdomain(PACKAGE, PATH_LOCALE);
	bind_textdomain_codeset(PACKAGE, "UTF-8");
	textdomain(PACKAGE);
#endif /* NLS */

#ifndef __WIN32__
	my_argc = argc;
	my_argv = argv;
#endif /* __WIN32__ */

/*
	test_space();
	exit(0);
*/

#ifdef __WIN32__
	WORD wVersionRequested = MAKEWORD(1, 1);	/* WinSock version */
	WSADATA data;					/* WinSock information structure */

	/* let's initialize WinSock */
	if (WSAStartup(wVersionRequested, &data) != 0) {
		error("Initialization of WinSock failed");
		error("Shutting down the game");
		exit(-1);
	}
#endif /* __WIN32__ */

#ifndef PUBLIC_SERVER
	game_start();
#else /* PUBLIC_SERVER */
	public_server_start();
#endif /* PUBLIC_SERVER */

	return 0;
}
