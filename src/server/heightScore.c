
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "main.h"
#include "list.h"
#include "textFile.h"
#include "heightScore.h"

static textFile_t *textFile;

void initHeightScore(char *file)
{
	int i;

	textFile = loadTextFile(file);

	if( textFile == NULL )
	{
		fprintf(stderr, _("I am unable to load: \"%s\" !\n"), file);
		fprintf(stderr, _("Creating: \"%s\"\n"), file);
		textFile = newTextFile(file);
	}
	else
	{
#ifdef DEBUG
		printf(_("Scorefile: \"%s\"\n"), file);
#endif
		return;
	}

	if( textFile == NULL )
	{
		fprintf(stderr, _("I was unable to create: \"%s\" !\n"), file);
		return;
	}

	for( i = 0 ; i < HEIGHTSCORE_MAX_PLAYERS ; i++ )
	{
		addList(textFile->text, strdup("no_name 0") );
	}

	saveTextFile(textFile);
}

int addPlayerInHighScore(char *name, int score)
{
	int i;

	if( score <= 0 )
	{
		return -1; // Ha ha ha
	}

	for( i = 0 ; i < HEIGHTSCORE_MAX_PLAYERS ; i++ )
	{
		char *line;
		char thisName[STR_NAME_SIZE];
		int thisCore;

		line = (char *) textFile->text->list[i];
		sscanf(line, "%s %d", thisName, &thisCore);

		if( score >= thisCore )
		{
			char new[STR_SIZE];
			
			sprintf(new, "%s %d", name, score);
			insList(textFile->text, i, strdup(new) );
			delListItem(textFile->text, HEIGHTSCORE_MAX_PLAYERS, free);
			//printTextFile(textFile);
			saveTextFile(textFile);

			return 0;
		}
	}

	return -1;
}

char* getTableItem(int index)
{
	if( textFile != NULL && 
	    index >= 0 &&
	    index < textFile->text->count )
	{
		return (char *)textFile->text->list[index];
	}

	return NULL;
}

void quitHeightScore()
{
	if( textFile != NULL )
	{
		saveTextFile(textFile);
		destroyTextFile(textFile);
	}
}
