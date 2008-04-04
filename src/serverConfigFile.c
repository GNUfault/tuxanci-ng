
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "main.h"
#include "list.h"
#include "textFile.h"
#include "serverConfigFile.h"

static textFile_t *serverTextFile;

static void prepareConfigFile(textFile_t *ts)
{
	int i;
	int j;

	for( i = 0 ; i < serverTextFile->text->count ; i++ )
	{
		char *line;
		int len;

		line = (char *)(serverTextFile->text->list[i]);
		len = strlen(line);

		for( j = 0 ; j < len ; j++ )
		{
			if( line[j] == '	' ) // [TAB]
			{
				line[j] = ' ';
			}
		}
	}

}

void initServerConfigFile()
{
	serverTextFile = loadTextFile("./server.conf");

	if( serverTextFile == NULL )
	{
		fprintf(stderr, "I dont load config file, I used default values\n");
		return;
	}
	
	prepareConfigFile(serverTextFile);

}
char* getServerConfigFileValue(char *env, char *s)
{
	int i;
	int len;

	if( serverTextFile == NULL )
	{
		return s;
	}

	len = strlen(env);

	for( i = 0 ; i < serverTextFile->text->count ; i++ )
	{
		char *line;

		line = (char *)(serverTextFile->text->list[i]);

		if( strncmp(line, env, len) == 0 )
		{
			return strrchr(line,' ')+1;
		}
	}

	return s;
}

void quitServerConfigFile()
{
	if( serverTextFile != NULL )
	{
		destroyTextFile(serverTextFile);
	}
}

 
