#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifdef SUPPORT_LIBPHYSFS
#include <physfs.h>
#endif /* SUPPORT_LIBPHYSFS */

#ifdef SUPPORT_LIBZIP
#include <zip.h>
#endif /* SUPPORT_LIBZIP */

#include "main.h"
#include "archive.h"

#define ARCHIVE_BUFFER_SIZE	4096
#define STR_TMP_FILE_NAME	4096

#ifdef SUPPORT_LIBZIP
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
	tmp = getenv("TMPDIR") == NULL ? "/tmp" : getenv("TMPDIR");
#else /* __WIN32__ */
	tmp = getenv("TEMP");
#endif /* __WIN32__ */
	sprintf(name, "%s%stuxanci.%d-%s", tmp, PATH_SEPARATOR, getpid(), filename);

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

		if (len > 0) {
			fwrite(buf, len, 1, file);
		}
	} while (len > 0);

	fclose(file);
	zip_fclose(zipFile);
	zip_close(zipArchive);

	return strdup(name);
}
#endif /* SUPPORT_LIBZIP */

#ifdef SUPPORT_LIBPHYSFS
char *archive_extract_file(char *archive, char *filename)
{
	char name[STR_TMP_FILE_NAME];
	char buffer[ARCHIVE_BUFFER_SIZE];

	PHYSFS_file *file_archive;
	FILE *file;
	int count;

	char *tmp;

#ifndef __WIN32__
	tmp = getenv("TMPDIR") == NULL ? "/tmp" : getenv("TMPDIR");
#else /* __WIN32__ */
	tmp = getenv("TEMP");
#endif /* __WIN32__ */
	sprintf(name, "%s%stuxanci.%d-%s", tmp, PATH_SEPARATOR, getpid(), filename);

	if (!(PHYSFS_init(get_program_name()))) {
		error("Unable to init physfs");
		return NULL;
	}

	if (!(PHYSFS_addToSearchPath(archive, 1))) {
		error("Unable to open archive [%s]", archive);
		PHYSFS_deinit();
		return NULL;
	}

	file_archive = PHYSFS_openRead(filename);

	if (file_archive == NULL) {
		error("Unable to extract the file [%s] from archive [%s]", filename, archive);
		PHYSFS_deinit();
		return NULL;
	}

	if ((file = fopen(name, "wb")) == NULL) {
		error("Unable to create temporary file [%s]", name);
		PHYSFS_close(file_archive);
		PHYSFS_deinit();
		return NULL;
	}

	while ((count = PHYSFS_read(file_archive, buffer, 1, ARCHIVE_BUFFER_SIZE)) > 0) {
		fwrite(buffer, count, 1, file);
	}

	PHYSFS_close(file_archive);
	fclose(file);

	PHYSFS_deinit();

	return strdup(name);
}
#endif /* SUPPORT_LIBPHYSFS */

#ifdef __EMSCRIPTEN__
char *archive_extract_file(char *archive, char *filename)
{
        char *name = malloc(STR_TMP_FILE_NAME);
        char *base = strrchr(archive, '/');
        base = base ? base + 1 : archive;
        char arenaName[256];
        strncpy(arenaName, base, sizeof(arenaName) - 1);
        char *dot = strstr(arenaName, ".zip");
        if (dot) *dot = '\0';
        sprintf(name, "/usr/local/share/tuxanci/arena-extracted/%s/%s", arenaName, filename);
        return name;
}
#endif /* __EMSCRIPTEN__ */

void archive_delete_file(char *s)
{
	unlink(s);
	free(s);
}
