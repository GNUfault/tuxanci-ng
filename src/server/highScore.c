
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "main.h"
#include "list.h"
#include "textFile.h"
#include "highScore.h"

static textFile_t *textFile;

void highScore_init(char *file)
{
	int i;

	textFile = textFile_load(file);

	if (textFile == NULL) {
		fprintf(stderr, _("I am unable to load: \"%s\"!\n"), file);
		fprintf(stderr, _("Creating: \"%s\"\n"), file);
		textFile = textFile_new(file);
	} else {

		DEBUG_MSG(_("Scorefile: \"%s\"\n"), file);
		return;
	}

	if (textFile == NULL) {
		fprintf(stderr, _("I was unable to create: \"%s\"!\n"), file);
		return;
	}

	for (i = 0; i < HIGHSCORE_MAX_PLAYERS; i++) {
		list_add(textFile->text, strdup("no_name 0"));
	}

	textFile_save(textFile);
}

int table_add(char *name, int score)
{
	int i;

	if (score <= 0) {
		return -1;				// Ha ha ha
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
			//textFile_print(textFile);
			textFile_save(textFile);

			return 0;
		}
	}

	return -1;
}

char *highScore_get_table(int index)
{
	if (textFile != NULL && index >= 0 && index < textFile->text->count) {
		return (char *) textFile->text->list[index];
	}

	return NULL;
}

void highScore_quit()
{
	if (textFile != NULL) {
		textFile_save(textFile);
		textFile_destroy(textFile);
	}
}
