
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "base/main.h"
#include "client/configFile.h"

int isYesOrNO(char *s)
{
	if( s[0] == 'Y' || s[0] == 'y' )return 1;
	return 0;
}

char* getYesOrNo(int n)
{
	return ( n != 0 ? "YES" : "NO" );
}

int getValue(char *line, char *env, char *val, int len)
{
	char *offset_env;
	char *offset_val_begin;
	char *offset_val_end;
	char clone_env[STR_SIZE];
	int val_len;

	strcpy(clone_env, " ");
	strcat(clone_env, env);

	offset_env = strstr(line, clone_env);
	if( offset_env == NULL )return -1;
	
	offset_val_begin = strchr(offset_env, '"');
	if( offset_val_begin == NULL )return -1;
	
	offset_val_end = strchr(offset_val_begin+1, '"');
	if( offset_val_end == NULL )return -1;
	
	val_len = (int)(offset_val_end - ( offset_val_begin + 1) );
	if( val_len > len - 1 ) val_len = len - 1;

	memset(val, 0, len);
	memcpy(val, offset_val_begin+1, val_len);

	return 0;
}

char* setValue(char *line, char *env, char *val)
{
	char *offset_env;
	char *offset_val_begin;
	char *offset_val_end;
	char clone_env[STR_SIZE];
	char clone[STR_SIZE];

	//strcpy(clone_env, " ");
	strcpy(clone_env, env);

	offset_env = strstr(line, clone_env);
	if( offset_env == NULL )return NULL;
	
	offset_val_begin = strchr(offset_env, '"');
	if( offset_val_begin == NULL )return NULL;
	
	offset_val_end = strchr(offset_val_begin+1, '"');
	if( offset_val_end == NULL )return NULL;
	
	memset(clone, 0, STR_SIZE);
	strncpy(clone, line, (int) ( (offset_val_begin - line) + 1 ) );
	strcat(clone, val);
	strcat(clone, offset_val_end);

	return strdup( clone );
}

int getValueInConfigFile(textFile_t *textFile, char *env, char *val, int len)
{
	int i;
	char *line;
	
	for( i = 0 ; i < textFile->text->count ; i++ )
	{
		line = (char *) (textFile->text->list[i]);
		
		if( strstr(line, env) != NULL )
		{
			return getValue(line, env, val, len);
		}
	}

	return -1;
}

int setValueInConfigFile(textFile_t *textFile, char *env, char *val)
{
	int i;
	char *line;
	char *ret;
	
	for( i = 0 ; i < textFile->text->count ; i++ )
	{
		line = (char *) (textFile->text->list[i]);
		
		if( strstr(line, env) != NULL )
		{
			ret = setValue(line, env, val);

			if( ret != NULL )
			{
				delListItem(textFile->text, i, free);
				insList(textFile->text, i, ret);

				return 0;
			}
		}
	}

	return -1;
}

void loadValueFromConfigFile(textFile_t *textFile, char *env, char *val, int len, char *butVal)
{
	strcpy(val, butVal);

	if( getValueInConfigFile(textFile, env, val, len) != 0 )
	{
		char line[STR_SIZE];

		sprintf(line, " %s=\"%s\"", env, butVal);
		addList(textFile->text, strdup(line) );

		printf("Add line \"%s\" in config file.\n", line);
	}
}
