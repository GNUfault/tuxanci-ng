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
		error("Environment variable HOME not found");
		exit(0);
	}

	sprintf(homeDirector, "%s%s%s", envHome, PATH_SEPARATOR, HOMEDIRECTOR_NAME);

	if (access(homeDirector, F_OK) != 0) {
		debug("Creating home directory [%s]", homeDirector);

#ifndef __WIN32__
		if (mkdir(homeDirector, 0755) != 0) {
#else /* __WIN32__ */
		if (mkdir(homeDirector) != 0) {
#endif /* __WIN32__ */
			error("Unable to create home directory [%s]", homeDirector);
			exit(0);
		}
	}
}

char *home_director_get()
{
	return homeDirector;
}
