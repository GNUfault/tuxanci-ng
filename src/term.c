
#include <stdlib.h>
#include <assert.h>

#include "main.h"
#include "list.h"
#include "interface.h"
#include "font.h"
#include "term.h"
#include "myTimer.h"

typedef struct struct_term_t
{
	char *text;
	my_time_t delTime;
} term_t;

static list_t *listText;

static term_t* newTermItem(char *s)
{
	term_t *new;
	char *line;
	int len;

	assert( s != NULL );
	
	line = strdup(s);
	len = strlen(line);
	line[len-1] = '\0';

	new = malloc( sizeof(term_t) );
	memset(new, 0, sizeof(term_t));

	new->text = line;
	new->delTime = getMyTime() + TERM_DEL_TEXT_TIMEOUT;

	return new;
}

static void destroyTermItem(term_t *p)
{
	assert( p != NULL );
	
	free(p->text);
	free(p);
}

void initTerm()
{
	listText = newList();
}

void drawTerm()
{
	int i;

	for( i = 0 ; i < listText->count ; i++ )
	{
		char *line;

		line = (char *)( (term_t *)listText->list[i] )->text;
		drawFont(line, 10, 10 + i*20, COLOR_WHITE);
	}
}

void eventTerm()
{
	my_time_t currentTime;
	int i;

	currentTime = getMyTime();


	for( i = 0 ; i < listText->count ; i++ )
	{
		my_time_t thisTime;

		thisTime = ( (term_t *)listText->list[i] )->delTime;

		if( currentTime > thisTime )
		{
			delListItem(listText, i, destroyTermItem);
			i--;
		}
	}

}

void appendTextInTerm(char *s)
{

	addList(listText, newTermItem(s) );
	
	if( listText->count > TERM_MAX_LINES )
	{
		delListItem(listText, 0, destroyTermItem);
	}
}

void quitTerm()
{
	assert( listText != NULL );
	destroyListItem(listText, destroyTermItem);
}
