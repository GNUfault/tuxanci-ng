
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "main.h"
#include "list.h"
#include "homeDirector.h"
#include "textFile.h"
#include "configFile.h"
#include "language.h"

static textFile_t *languageFile;
static char fontFile[STR_FILE_NAME_SIZE];
static int fontSize;

char* getHead(textFile_t *ts)
{
	int i;

	for( i = 0 ; i < ts->text->count ; i++ )
	{
		char *line;
		line = (char *) (ts->text->list[i]);

		if( strncmp(line, "HEAD", 4) == 0 )
		{
			return line;
		}
	}

	return NULL;
}


int initLanguage()
{
	textFile_t *languageTypeFile;
	char path[STR_PATH_SIZE];
	char *lang;
	char *head;

	sprintf(path, "%s/lang.conf", getHomeDirector());
	languageTypeFile = loadTextFile(path);

	if( languageTypeFile == NULL )
	{
		fprintf(stderr, "Don't load %s\n", path);
		fprintf(stderr, "I create %s\n", path);

		languageTypeFile = newTextFile(path);
		addList(languageTypeFile->text, strdup("en") );
		saveTextFile(languageTypeFile);
	}

	if( languageTypeFile == NULL )
	{
		fprintf(stderr, "Don't create %s\n", path);
		return -1;
	}

	if( languageTypeFile->text->count == 0 )
	{
		fprintf(stderr, "File %s empty\n", path);
		return -1;
	}

	lang = ((char *)languageTypeFile->text->list[0]);
	printf("select lang %s\n", lang);

	sprintf(path, PATH_LANG "%s.lang", lang);
	languageFile = loadTextFile(path);

	if( languageFile == NULL )
	{
		fprintf(stderr, "Don't load %s\n", path);
		return -1;
	}

	head = getHead(languageFile);

	if( head != NULL )
	{
		char val[STR_SIZE];
	
		if( getValue(head, "fontfile", val, STR_SIZE) == 0 )
		{
			strcpy(fontFile, val);
		}
		else
		{
			fprintf(stderr, "fontfile in lang file %s not found\n", path);
			return -1;
		}

		if( getValue(head, "fontsize", val, STR_SIZE) == 0 )
		{
			fontSize = atoi(val);
		}
		else
		{
			fprintf(stderr, "fontsize in lang file %s not found\n", path);
			return -1;
		}
	}
	else
	{
		fprintf(stderr, "head in lang file %s not found\n", path);
		return -1;
	}

	return 0;
}

char* getMyText(char *key)
{
	int i;
	int len;

	len = strlen(key);

	for( i = 0 ; i < languageFile->text->count ; i++ )
	{
		char *line;
		line = (char *) (languageFile->text->list[i]);

		if( strncmp(key, line, len) == 0 )
		{
			return line+strlen(key)+1;
		}
	}

	return NULL;
}

char* getLanguageFont()
{
	return fontFile;
}

int getLanguageSize()
{
	return fontSize;
}

void quitLanguage()
{
	destroyTextFile(languageFile);
}

