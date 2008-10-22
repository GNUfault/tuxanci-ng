
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
	char *configFile;

	configFile = getParamElse("--config-file", SERVER_CONFIG);
	printf(_("Loading configuration from: \"%s\"\n"), getParamElse("--config-file", SERVER_CONFIG) );

	serverTextFile = loadTextFile(configFile);

	if( serverTextFile == NULL )
	{
		fprintf(stderr, _("I was unable to load config file. Falling back to defaults!\n"));
		return;
	}
	
	prepareConfigFile(serverTextFile);
}

static char *findValue(char *s)
{
	int len;
	int i;

	len = strlen(s);

	for( i = 0 ; i < len-1 ; i++)
	{
		if( s[i] == ' ' && s[i+1] != ' ' )
		{
			return s+i+1;
		}
	}

	return NULL;
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
			return findValue(line);
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

 
