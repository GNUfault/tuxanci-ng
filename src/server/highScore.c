#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "main.h"
#include "list.h"
#include "textFile.h"
#include "highScore.h"

static textFile_t *textFile;

void high_score_init(char *file)
{
	int i;

	textFile = text_file_load(file);

	if (textFile == NULL) {
		fprintf(stderr, _("[Error] Unable to load high score [%s]\n"), file);
		fprintf(stderr, _("[Debug] Creating high score file [%s]\n"), file);
		textFile = text_file_new(file);
	} else {
		DEBUG_MSG(_("[Debug] Loading high score file [%s]\n"), file);
		return;
	}

	if (textFile == NULL) {
		fprintf(stderr, _("[Error] Unable to create high score file [%s]\n"), file);
		return;
	}

	for (i = 0; i < HIGHSCORE_MAX_PLAYERS; i++) {
		list_add(textFile->text, strdup("--- 0"));
	}

	text_file_save(textFile);
}

int table_add(char *name, int score)
{
	int i;

	if (score <= 0) {
		return -1;	/* Ha ha ha */
	}

	for (i = 0; i < HIGHSCORE_MAX_PLAYERS; i++) {
		char *line;
		char thisName[STR_NAME_SIZE];
		int thisCore;

		line = (char *) textFile->text->list[i];
		sscanf(line, "%s %d", thisName, &thisCore);

		if (score >= thisCore) {
			char new[STR_SIZE];

			sprintf(new, "%s %d", name, score);
			list_ins(textFile->text, i, strdup(new));
			list_del_item(textFile->text, HIGHSCORE_MAX_PLAYERS, free);
			/*text_file_print(textFile);*/
			text_file_save(textFile);

			return 0;
		}
	}

	return -1;
}

char *high_score_get_table(int index)
{
	if (textFile != NULL && index >= 0 && index < textFile->text->count) {
		return (char *) textFile->text->list[index];
	}

	return NULL;
}

void high_score_quit()
{
	if (textFile != NULL) {
		text_file_save(textFile);
		text_file_destroy(textFile);
	}
}
