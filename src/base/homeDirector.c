
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <sys/stat.h>

#include "base/main.h"
#include "base/homeDirector.h"

static char homeDirector[STR_PATH_SIZE];

void createHomeDirector()
{
	char *envHome;

	envHome = getenv("HOME");

	if( envHome == NULL )
	{
		fprintf(stderr, "Environment HOME not found !\n");
		exit(0);
	}

	sprintf(homeDirector, "%s/%s", envHome, HOMEDIRECTOR_NAME);

	if ( access(homeDirector, F_OK) != 0 )
	{
		printf("Create home director %s\n", homeDirector);

		if( mkdir(homeDirector, 0755) != 0 )
		{
			fprintf(stderr, "Can't create home directory!\n");
			exit(0);
		}
	}
}

char *getHomeDirector()
{
	return homeDirector;
}
