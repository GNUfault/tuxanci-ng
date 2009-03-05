
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <zip.h>

#include "archive.h"

#define ARCHIVE_BUFFER_SIZE	4096
#define STR_TMP_FILE_NAME	4096

char *extractFileFromArchive(char *archive, char *filename)
{
	char name[STR_TMP_FILE_NAME];
	char buf[ARCHIVE_BUFFER_SIZE];

	struct zip *zipArchive;
	struct zip_file *zipFile;

	FILE *file;
	int len;
	int err;

	sprintf(name, "/tmp/tuxanci%d-%s", getpid(), filename);

	//printf("ARCHIVE %s %s %s\n", archive, filename, name);

	zipArchive = zip_open(archive, 0, &err);

	if( zipArchive == NULL )
	{
		fprintf(stderr, "Archive not open !\n");
		return NULL;
	}

	zipFile = zip_fopen(zipArchive, filename, ZIP_FL_UNCHANGED);

	if( zipFile == NULL )
	{
		fprintf(stderr, "File from archive not open !\n");
		zip_close(zipArchive);
		return NULL;
	}

	if( (file = fopen(name, "wb") ) == NULL)
	{
		fprintf(stderr, "No do not create temp file %s\n", name);
		zip_fclose(zipFile);
		zip_close(zipArchive);
		return NULL;
	}

	do {
		len = zip_fread(zipFile, buf, ARCHIVE_BUFFER_SIZE);
		//printf("len = %d\n", len);

		if( len > 0 )
		{
			fwrite(buf, len, 1, file);
		}
	} while( len > 0 );

	fclose(file);
	zip_fclose(zipFile);
	zip_close(zipArchive);

	return strdup(name);
}

void deleteExtractFile(char *s)
{
	unlink(s);
	free(s);
}
