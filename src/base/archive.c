
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <zzip/zzip.h>

#include "archive.h"

#define ARCHIVE_BUFFER_SIZE	1024
#define STR_TMP_FILE_NAME	4096

char *extractFileFromArchive(char *archive, char *filename)
{
	char buf[ARCHIVE_BUFFER_SIZE];
	char archive_name[STR_TMP_FILE_NAME];
	char name[STR_TMP_FILE_NAME];

	ZZIP_FILE *zip_file;
	FILE *file;
	zzip_ssize_t len;

	strcpy(archive_name, archive);
	sprintf(strrchr(archive_name, '.'), "/%s", filename);

	sprintf(name,"/tmp/tuxanci%d-%s", getpid(), filename);

	if( (zip_file = zzip_open(archive_name, O_RDONLY)) == NULL )
	{
		fprintf(stderr, "No do not open %s archive\n", archive_name);
		return NULL;
	}

	if( (file = fopen(name, "wb")) == NULL )
	{
		fprintf(stderr, "No do not create temp file %s\n", name);
		return NULL;
	}

	do{
		len = zzip_read(zip_file, buf, ARCHIVE_BUFFER_SIZE);
		//printf("len = %d\n", len);

		if( len > 0 )
		{
			fwrite(buf, len, 1, file);
		}
	}while( len > 0 );

	zzip_close(zip_file);
	fclose(file);

	return strdup(name);
}

void deleteExtractFile(char *s)
{
	unlink(s);
	free(s);
}

#if ARCHIVE_TEST
int main()
{
	char *name;

	name = extractFileFromArchive("hello/a/b/hello");
	printf("name = %s\n", name);
	deleteExtractFile(name);
	return 0;
}
#endif

