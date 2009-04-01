
#ifndef FONT_CONFIG_H

#define FONT_CONFIG_H

typedef struct font_config_struct
{
	char *path;
	char *flag;
} font_config_t;

extern int fontconfig_init();
extern font_config_t* fontconfig_find(char **arg);
extern void fontconfig_del_list(font_config_t *list);
extern int fontconfig_quit();

#endif
