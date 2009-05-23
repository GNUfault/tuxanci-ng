#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fontconfig/fontconfig.h>

#include "main.h"
#include "fontConfig.h"

int fontconfig_init()
{
	int res;
	
	res = FcInit();
	
	if (res == 0) {
		fprintf(stderr, _("[Error] Unable to init fontconfig library\n"));
		return 1;
	}

	return 0;
}

font_config_t *fontconfig_find(char **arg)
{
	FcObjectSet *os;
	FcFontSet *fs;
	FcPattern *pat;
	font_config_t *ret;
	int i;

	os = NULL;
	ret = NULL;

	for (i = 0; arg[i] != NULL; i++) {
		if (i == 0) {
			pat = FcNameParse((FcChar8 *) arg[i]);
		}

		if (os == NULL) {
			os = FcObjectSetCreate();
		}

		FcObjectSetAdd(os, arg[i]);
	}

	if (os == NULL) {
		os = FcObjectSetBuild(FC_FAMILY, FC_STYLE, (char *) 0);
	}

	fs = FcFontList(0, pat, os);
	FcObjectSetDestroy(os);

	if (pat != NULL) {
		FcPatternDestroy(pat);
	}

	if (fs != NULL) {
		int j;

		ret = malloc((fs->nfont + 1) * sizeof(font_config_t));

		for (j = 0; j < fs->nfont; j++) {
			FcChar8 *font;
			FcChar8 *file;
		
			font = FcNameUnparse(fs->fonts[j]);

			if (FcPatternGetString(fs->fonts[j], FC_FILE, 0, &file) == FcResultMatch) {
				ret[j].path = strdup((char *) file);
			}

			ret[j].flag = strdup((char *) font);
			free(font);
		}

		ret[fs->nfont].path = NULL;
		ret[fs->nfont].flag = NULL;

		FcFontSetDestroy(fs);
	}

	return ret;
}

void fontconfig_del_list(font_config_t *list)
{
	int i;

	for (i = 0; list[i].path != NULL; i++) {
		free(list[i].path);
		free(list[i].flag);
	}

	free(list);
}

int fontconfig_quit()
{
	FcFini();
	return 0;
}
