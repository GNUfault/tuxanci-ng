#include <stdlib.h>
#include <string.h>

#include "main.h"
#include "fontConfig.h"

static font_config_t *s_stub_list;

int fontconfig_init()
{
	return 0;
}

font_config_t *fontconfig_find(char **arg)
{
	(void)arg;
	if (s_stub_list != NULL) {
		return s_stub_list;
	}
	s_stub_list = malloc(2 * sizeof(font_config_t));
	s_stub_list[0].path = strdup("/font.ttf");
	s_stub_list[0].flag = strdup("DejaVu Sans");
	s_stub_list[1].path = NULL;
	s_stub_list[1].flag = NULL;
	return s_stub_list;
}

void fontconfig_del_list(font_config_t *list)
{
	int i;
	if (list == NULL) {
		return;
	}
	for (i = 0; list[i].path != NULL; i++) {
		free(list[i].path);
		free(list[i].flag);
	}
	free(list);
	if (list == s_stub_list) {
		s_stub_list = NULL;
	}
}

int fontconfig_quit()
{
	return 0;
}
