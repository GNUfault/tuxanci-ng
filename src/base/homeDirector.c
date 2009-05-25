#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <sys/stat.h>

#include "main.h"
#include "homeDirector.h"

static char homeDirector[STR_PATH_SIZE];

void home_director_create()
{
	char *envHome;
#ifndef __WIN32__
	envHome = getenv("HOME");
#else /* __WIN32__ */
	envHome = getenv("USERPROFILE");
#endif /* __WIN32__ */
	if (envHome == NULL) {
		fprintf(stderr, _("[Error] Environment variable HOME not found\n"));
		exit(0);
	}

	sprintf(homeDirector, "%s/%s", envHome, HOMEDIRECTOR_NAME);

	if (access(homeDirector, F_OK) != 0) {
		DEBUG_MSG(_("[Debug] Creating home directory [%s]\n"), homeDirector);

#ifndef __WIN32__
		if (mkdir(homeDirector, 0755) != 0) {
#else /* __WIN32__ */
		if (mkdir(homeDirector) != 0) {
#endif /* __WIN32__ */
			fprintf(stderr, _("[Error] Unable to create home directory [%s]\n"), homeDirector);
			exit(0);
		}
	}
}

char *home_director_get()
{
	return homeDirector;
}
