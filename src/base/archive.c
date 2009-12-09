#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <zip.h>

#include "main.h"
#include "archive.h"

#define ARCHIVE_BUFFER_SIZE	4096
#define STR_TMP_FILE_NAME	4096

char *archive_extract_file(char *archive, char *filename)
{
	char name[STR_TMP_FILE_NAME];
	char buf[ARCHIVE_BUFFER_SIZE];

	struct zip *zipArchive;
	struct zip_file *zipFile;

	FILE *file;
	int len;
	int err;
	char *tmp;

#ifndef __WIN32__
	tmp = getenv("TMPDIR");
#else
	tmp = getenv("TEMP");
#endif
	sprintf(name, "%s%stuxanci.%d-%s", tmp, PATH_SEPARATOR ,getpid(), filename);

	/*printf("ARCHIVE %s %s %s\n", archive, filename, name);*/

	zipArchive = zip_open(archive, 0, &err);

	if (zipArchive == NULL) {
		error("Unable to open archive [%s]", archive);
		return NULL;
	}

	zipFile = zip_fopen(zipArchive, filename, ZIP_FL_UNCHANGED);

	if (zipFile == NULL) {
		error("Unable to extract the file [%s] from archive [%s]", filename, archive);
		zip_close(zipArchive);
		return NULL;
	}

	if ((file = fopen(name, "wb")) == NULL) {
		error("Unable to create temporary file [%s]", name);
		zip_fclose(zipFile);
		zip_close(zipArchive);
		return NULL;
	}

	do {
		len = zip_fread(zipFile, buf, ARCHIVE_BUFFER_SIZE);
		/*printf("len = %d\n", len);*/

		if (len > 0) {
			fwrite(buf, len, 1, file);
		}
	} while (len > 0);

	fclose(file);
	zip_fclose(zipFile);
	zip_close(zipArchive);

	return strdup(name);
}

void archive_delete_file(char *s)
{
	unlink(s);
	free(s);
}
