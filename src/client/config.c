#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "main.h"
#include "list.h"
#include "homeDirector.h"
#include "textFile.h"
#include "configFile.h"
#include "config.h"

typedef struct setting_int_struct {
	int key;		/* identification key */
	char *param;		/* parameter of the program */
	char *var;		/* variable in the config file */
	int type;		/* type of the data - string/number */
	int value_int;		/* numeric data */
	char *value_str;	/* textual data */
} config_t;

#define CONFIG_TYPE_INT	1
#define CONFIG_TYPE_STR	2

static config_t config_list[] =
{
	{ .key=CFG_NAME_RIGHT,	.var="NAME_PLAYER_RIGHT",	.param="--name-right",	.type=CONFIG_TYPE_STR,	.value_str="name1" },
	{ .key=CFG_NAME_LEFT,	.var="NAME_PLAYER_LEFT",	.param="--name-left",	.type=CONFIG_TYPE_STR,	.value_str="name2" },
	{ .key=CFG_COUNT_ROUND,	.var="COUNT_ROUND",		.param="--count",	.type=CONFIG_TYPE_INT,	.value_int=15 },
	{ .key=CFG_AI,		.var="AI",			.param="--ai",		.type=CONFIG_TYPE_INT,	.value_int=0 },
	{ .key=CFG_MUSIC,	.var="MUSIC",			.param="--music",	.type=CONFIG_TYPE_INT,	.value_int=1 },
	{ .key=CFG_SOUND,	.var="SOUND",			.param="--sound",	.type=CONFIG_TYPE_INT,	.value_int=1 },
	{ .key=CFG_ARENA,	.var="ARENA",			.param="--arena",	.type=CONFIG_TYPE_STR,	.value_str="FAGN" },

	{ .key=CFG_GUN_DUAL_SIMPLE,	.var="GUN_DUAL_SIMPLE",		.param="--gun-dual-simple",	.type=CONFIG_TYPE_INT,	.value_int=1 },
	{ .key=CFG_GUN_SCATTER,		.var="GUN_SCATTER",		.param="--gun-scatter",		.type=CONFIG_TYPE_INT,	.value_int=1 },
	{ .key=CFG_GUN_TOMMY,		.var="GUN_TOMMY",		.param="--gun-tommy",		.type=CONFIG_TYPE_INT,	.value_int=1 },
	{ .key=CFG_GUN_LASSER,		.var="GUN_LASSER",		.param="--gun-lasser",		.type=CONFIG_TYPE_INT,	.value_int=1 },
	{ .key=CFG_GUN_MINE,		.var="GUN_MINE",		.param="--gun-mine",		.type=CONFIG_TYPE_INT,	.value_int=1 },
	{ .key=CFG_GUN_BOMBBALL,	.var="GUN_BOMBBALL",		.param="--gun-bombball",	.type=CONFIG_TYPE_INT,	.value_int=1 },

	{ .key=CFG_BONUS_SPEED,		.var="BONUS_SPEED",		.param="--bonus-speed",		.type=CONFIG_TYPE_INT,	.value_int=1 },
	{ .key=CFG_BONUS_SHOT,		.var="BONUS_SHOT",		.param="--bonus-shot",		.type=CONFIG_TYPE_INT,	.value_int=1 },
	{ .key=CFG_BONUS_TELEPORT,	.var="BONUS_TELEPORT",		.param="--bonus-teleport",	.type=CONFIG_TYPE_INT,	.value_int=1 },
	{ .key=CFG_BONUS_GHOST,		.var="BONUS_GHOST",		.param="--bonus-ghost",		.type=CONFIG_TYPE_INT,	.value_int=1 },
	{ .key=CFG_BONUS_4X,		.var="BONUS_4X",		.param="--bonus-4x",		.type=CONFIG_TYPE_INT,	.value_int=1 },
	{ .key=CFG_BONUS_HIDDEN,	.var="BONUS_HIDDEN",		.param="--bonus-hidden",	.type=CONFIG_TYPE_INT,	.value_int=1 },

	{ .key=CFG_GAME_TYPE,		.var="GAME_TYPE",		.param="--game-type",		.type=CONFIG_TYPE_STR,	.value_str="none" },
	{ .key=CFG_NET_IP,		.var="NET_IP",			.param="--ip",			.type=CONFIG_TYPE_STR,	.value_str="127.0.0.1" },
	{ .key=CFG_NET_PORT,		.var="NET_PORT",		.param="--port",		.type=CONFIG_TYPE_STR,	.value_str="6800" }
};

static textFile_t *config_file;

static config_t *find_config(int key)
{
	int i;

	for (i = 0; i < CONFIG_LIST_COUNT; i++) {
		if (config_list[i].key == key) {
			return &config_list[i];
		}
	}

	return NULL;
}

static void check_param()
{
	int i;

	for (i = 0; i < CONFIG_LIST_COUNT; i++) {
		char *str;

		str = getParam(config_list[i].param);

		if (str != NULL) {
			if (config_list[i].type == CONFIG_TYPE_INT) {
				config_list[i].value_int = atoi(str);
			} else {
				if (config_list[i].value_str != NULL) {
					free(config_list[i].value_str);
				}

				config_list[i].value_str = strdup(str);
			}
		}
	}
}

int config_init()
{
	config_load();
	check_param();

	return 0;
}

void config_load()
{
	char path[STR_PATH_SIZE];
	char val[STR_SIZE];
	int i;

	sprintf(path, "%s/tuxanci.conf", home_director_get());

	config_file = text_file_load(path);

	if (config_file == NULL) {
		fprintf(stderr, _("[Error] Unable to load config file [%s]\n"), path);
		fprintf(stderr, _("[Debug] Creating config file [%s]\n"), path);

		config_file = text_file_new(path);
	}

	if (config_file == NULL) {
		fprintf(stderr, _("[Error] Unable to create config file [%s]\n"), path);
		return;
	}

	for (i = 0; i < CONFIG_LIST_COUNT; i++) {
		if (config_list[i].type == CONFIG_TYPE_INT) {
			char str[STR_SIZE];

			sprintf(str, "%d", config_list[i].value_int);
			loadValueFromConfigFile(config_file, config_list[i].var, val, STR_SIZE, str);
			config_list[i].value_int = atoi(val);
		} else {
			loadValueFromConfigFile(config_file, config_list[i].var, val, STR_SIZE, config_list[i].value_str);
			config_list[i].value_str = strdup(val);
		}
	}
}

void config_save()
{
	int i;

	if (config_file == NULL) {
		return;
	}

	for (i = 0; i < CONFIG_LIST_COUNT; i++) {
		if (config_list[i].type == CONFIG_TYPE_INT) {
			char str[STR_SIZE];

			sprintf(str, "%d", config_list[i].value_int);
			setValueInConfigFile(config_file, config_list[i].var, str);
		} else {
			setValueInConfigFile(config_file, config_list[i].var, config_list[i].value_str);
		}
	}

	text_file_save(config_file);
}

int config_get_int_value(int key)
{
	config_t *tmp;

	tmp = find_config(key);

	assert(tmp != NULL);
	assert(tmp->type == CONFIG_TYPE_INT);

	return tmp->value_int;
}

char *config_get_str_value(int key)
{
	config_t *tmp;

	tmp = find_config(key);

	assert(tmp != NULL);
	assert(tmp->type == CONFIG_TYPE_STR);

	return tmp->value_str;
}

void config_set_int_value(int key, int n)
{
	config_t *tmp;

	tmp = find_config(key);

	assert(tmp != NULL);
	assert(tmp->type == CONFIG_TYPE_INT);

	tmp->value_int = n;
}

void config_set_str_value(int key, char *str)
{
	config_t *tmp;

	tmp = find_config(key);

	assert(tmp != NULL);
	assert(tmp->type == CONFIG_TYPE_STR);

	if (tmp->value_str != NULL) {
		free(tmp->value_str);	/* has to be allocated */
	}

	tmp->value_str = strdup(str);
}

int config_quit()
{
	int i;

	config_save();

	if (config_file != NULL) {
		text_file_destroy(config_file);
	}

	for (i = 0; i < CONFIG_LIST_COUNT; i++) {
		if (config_list[i].type == CONFIG_TYPE_STR && config_list[i].value_str != NULL) {
			free(config_list[i].value_str);
		}
	}

	return 0;
}
