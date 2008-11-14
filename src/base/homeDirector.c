
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <sys/stat.h>

#include "main.h"
#include "homeDirector.h"

static char homeDirector[STR_PATH_SIZE];

void createHomeDirector()
{
	char *envHome;
#ifndef __WIN32__
	envHome = getenv("HOME");
#else
	envHome = getenv("USERPROFILE");
#endif
	if (envHome == NULL) {
		fprintf(stderr, _("Environment variable HOME not found!\n"));
		exit(0);
	}

	sprintf(homeDirector, "%s/%s", envHome, HOMEDIRECTOR_NAME);

	if (access(homeDirector, F_OK) != 0) {

		DEBUG_MSG(_("Creating home directory %s.\n"), homeDirector);

#ifndef __WIN32__
		if (mkdir(homeDirector, 0755) != 0) {
#else
		if (mkdir(homeDirector) != 0) {
#endif
			fprintf(stderr, _("Unable to create home directory!\n"));
			exit(0);
		}
	}
}

char *getHomeDirector()
{
	return homeDirector;
}
