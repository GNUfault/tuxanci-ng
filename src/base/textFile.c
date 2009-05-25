#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <assert.h>
#include <unistd.h>

#include "main.h"
#include "textFile.h"

textFile_t *text_file_new(char *s)
{
	textFile_t *new;
	char path[STR_PATH_SIZE];

	assert(s != NULL);

	if (isFillPath(s)) {
		strcpy(path, s);
	} else {
		getcwd(path, STR_PATH_SIZE);
		strcat(path, "/");
		strcat(path, s);
	}

	new = malloc(sizeof(textFile_t));
	new->file = strdup(path);
	new->text = list_new();

	return new;
}

static void createLine(list_t *list, char *p, int len)
{
	char *line;
	char *begin_line;
	char *end_line;
	int length_line = 0;

	assert(list != NULL);
	assert(p != NULL);
	assert(len >= 0);

	begin_line = p;

	do {
		end_line = memchr(begin_line, '\n', len);

		if (end_line == NULL) {
			break;
		}

		length_line = (int) (end_line - (begin_line));

		line = malloc((length_line + 1) * sizeof(char));
		memset(line, 0, (length_line + 1) * sizeof(char));
		strncpy(line, begin_line, length_line);

		list_add(list, line);
		len -= (length_line + 1);

		begin_line = end_line + 1;

	} while (begin_line != NULL);
}

/*
 * Loads file *s and returns him formatted to textFile_t*
 */
textFile_t *text_file_load(char *s)
{
	FILE *file;
	textFile_t *ret;
	char *p;
	int file_length = 0;
	struct stat buf;

	assert(s != NULL);

	if (lstat(s, &buf) < 0) {
		fprintf(stderr, _("[Error] Unable to get file status [%s]\n"), s);
		return NULL;
	}

	file_length = buf.st_size;
	if ((file = fopen(s, "rb")) == NULL) {
		fprintf(stderr, _("[Error] Unable to open file for reading [%s]\n"), s);
		return NULL;
	}

	p = malloc(file_length * sizeof(char));

	if (fread(p, file_length * sizeof(char), 1, file) != 1) {
		fprintf(stderr, _("[Error] Unable to read data from file [%s]\n"), s);
		fclose(file);
		return NULL;
	}

	fclose(file);

	ret = text_file_new(s);
	createLine(ret->text, p, file_length);

	free(p);

	return ret;
}

/*
 * Prints the text saved in *p into stdout
 */
void text_file_print(textFile_t *p)
{
	int i;

	assert(p != NULL);

	printf(_("[Debug] Printing file [%s]\n\n"), p->file);

	for (i = 0; i < p->text->count; i++) {
		printf("%3d >> %s\n", i, (char *) p->text->list[i]);
	}
}

void text_file_save(textFile_t *p)
{
	int i;
	FILE *file;

	assert(p != NULL);

	file = fopen(p->file, "wb");

	for (i = 0; i < p->text->count; i++) {
		fprintf(file, "%s\n", (char *) p->text->list[i]);
	}

	fclose(file);
}

/*
 * Removes text saved in *p from the memory
 */
void text_file_destroy(textFile_t *p)
{
	assert(p != NULL);

	free(p->file);
	list_destroy_item(p->text, free);
	free(p);
}
