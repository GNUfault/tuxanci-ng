
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "main.h"
#include "list.h"
#include "homeDirector.h"
#include "textFile.h"
#include "language.h"

static textFile_t *languageFile;

void initLanguage()
{
	textFile_t *languageTypeFile;
	char path[STR_PATH_SIZE];
	char *lang;

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
		return;
	}

	if( languageTypeFile->text->count == 0 )
	{
		fprintf(stderr, "File %s empty\n", path);
		return;
	}

	lang = ((char *)languageTypeFile->text->list[0]);
	printf("select lang %s\n", lang);

	sprintf(path, PATH_DATA "lang.%s", lang);
	languageFile = loadTextFile(path);

	if( languageFile == NULL )
	{
		fprintf(stderr, "Don't load %s\n", path);
	}
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

void quitLanguage()
{
	destroyTextFile(languageFile);
}

