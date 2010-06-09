#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "main.h"
#include "list.h"
#include "director.h"
#include "homeDirector.h"
#include "net_multiplayer.h"
#include "tux.h"

#include "interface.h"
#include "screen.h"
#include "image.h"
#include "hotKey.h"
#include "config.h"

#ifndef NO_SOUND
#include "music.h"
#endif /* NO_SOUND */

#include "gameType.h"
#include "setting.h"

#include "widget.h"
#include "widget_label.h"
#include "widget_button.h"
#include "widget_image.h"
#include "widget_textfield.h"
#include "widget_choicegroup.h"
#include "widget_check.h"
#include "widget_select.h"

static widget_t *image_backgorund;

static widget_t *button_back;
static widget_t *button_play;
static widget_t *button_browser;

static widget_t *label_none;
static widget_t *label_server;
static widget_t *label_client;
static widget_t *label_load_session;

static widget_t *label_ip;
static widget_t *label_port;
static widget_t *label_session;

static list_t *listChoiceGroup;
static widget_t *check_none;
static widget_t *check_server;
static widget_t *check_client;
static widget_t *check_load_session;

static widget_t *textfield_ip;
static widget_t *textfield_port;

static widget_t *selectSession;

static void loadSession(widget_t *p)
{
	director_t *director;
	int i;

	director = director_load(home_director_get());

	select_remove_all(p);

	if (director == NULL) {
		return;
	}

	for (i = 0; i < director->list->count; i++) {
		char *line;

		line = director->list->list[i];

		if (strstr(line, ".sav") != NULL) {
			select_add(p, line);
		}
	}

	director_destroy(director);
}

static void hotkey_escape()
{
	screen_set("mainMenu");
}

void screen_startGameType()
{
#ifndef NO_SOUND
	music_play("menu", MUSIC_GROUP_BASE);
#endif /* NO_SOUND */

/*
	choiceGroup_set_status(check_none, TRUE);
	choiceGroup_set_status(check_server, FALSE);
	choiceGroup_set_status(check_client, FALSE);
	choiceGroup_set_status(check_load_session, FALSE);
*/

	loadSession(selectSession);

	hot_key_register(SDLK_ESCAPE, hotkey_escape);
}

void game_type_draw()
{
	int i;

	wid_image_draw(image_backgorund);

	for (i = 0; i < listChoiceGroup->count; i++) {
		widget_t *this;

		this = (widget_t *) listChoiceGroup->list[i];
		choice_group_draw(this);
	}

	label_draw(label_none);
	label_draw(label_server);
	label_draw(label_client);
	label_draw(label_load_session);

	if (choice_group_get_status(check_server) == TRUE || choice_group_get_status(check_client) == TRUE) {
		label_draw(label_port);
		text_field_draw(textfield_port);
		label_draw(label_ip);
		text_field_draw(textfield_ip);
	}

	if (choice_group_get_status(check_load_session) == TRUE) {
		label_draw(label_session);
		select_draw(selectSession);
	}
/*	
	if (check_server->status == TRUE) {
		game_type_set_ip(getParamElse("--ip", "127.0.0.1"));
		game_type_set_port(atoi(getParamElse("--port", "2200")));
	}
*/	

	if (choice_group_get_status(check_client) == TRUE) {
		button_draw(button_browser);
	}

	button_draw(button_back);
	button_draw(button_play);
}

void game_type_event()
{
	int i;

	for (i = 0; i < listChoiceGroup->count; i++) {
		widget_t *this;

		this = (widget_t *) listChoiceGroup->list[i];
		choice_group_event(this);
	}

	if (choice_group_get_status(check_server) == TRUE || choice_group_get_status(check_client) == TRUE) {
		text_field_event(textfield_ip);
		text_field_event(textfield_port);
	}

	if (choice_group_get_status(check_load_session) == TRUE) {
		select_event(selectSession);
	}

	button_event(button_back);
	button_event(button_play);

	if (choice_group_get_status(check_client) == TRUE) {
		button_event(button_browser);
	}
}

void stopScreenGameType()
{
	hot_key_unregister(SDLK_ESCAPE);
}

static void eventWidget(void *p)
{
	widget_t *button;

	button = (widget_t *) p;

	if (button == button_back) {
		screen_set("mainMenu");
		return;
	}

	if (button == button_play) {
		char *str;

		str = game_type_load_session();

		if (str != NULL) {
			screen_set("world");
			return;
		}

		if (public_server_get_settingGameType() == NET_GAME_TYPE_CLIENT) {
			screen_set("downArena");
		} else {
			screen_set("chiceArena");
		}

		return;
	}

	if (button == button_browser) {
		screen_set("browser");
		return;
	}
}

void game_type_init()
{
	image_t *image;

	image = image_get(IMAGE_GROUP_BASE, "screen_main");
	image_backgorund = wid_image_new(0, 0, image);

	button_back = button_new(_("Back"), 100, WINDOW_SIZE_Y - 100, eventWidget);
	button_play = button_new(_("Play"), WINDOW_SIZE_X - 200, WINDOW_SIZE_Y - 100, eventWidget);
	button_browser = button_new(_("Browser"), 300, 345, eventWidget);

	listChoiceGroup = list_new();
	check_none = choice_group_new(100, 150, FALSE, listChoiceGroup, eventWidget);
	check_server = choice_group_new(100, 200, FALSE, listChoiceGroup, eventWidget);
	check_client = choice_group_new(100, 250, FALSE, listChoiceGroup, eventWidget);
	check_load_session = choice_group_new(100, 300, FALSE, listChoiceGroup, eventWidget);

	if (strcmp(config_get_str_value(CFG_GAME_TYPE), "server") == 0) {
		choiceGroup_set_status(check_server, TRUE);
	} else if (strcmp(config_get_str_value(CFG_GAME_TYPE), "client") == 0) {
		choiceGroup_set_status(check_client, TRUE);
	} else if (strcmp(config_get_str_value(CFG_GAME_TYPE), "none") == 0) {
		choiceGroup_set_status(check_none, TRUE);
	} else {
		choiceGroup_set_status(check_load_session, TRUE);
	}

	label_none = label_new(_("Local game"), 130, 145, WIDGET_LABEL_LEFT);
	label_server = label_new(_("Set up a server"), 130, 195, WIDGET_LABEL_LEFT);
	label_client = label_new(_("Network game"), 130, 245, WIDGET_LABEL_LEFT);
	label_load_session = label_new(_("Load saved game"), 130, 295, WIDGET_LABEL_LEFT);

	label_ip = label_new(_("IP"), 300, 145, WIDGET_LABEL_LEFT);
	label_port = label_new(_("Port"), 300, 245, WIDGET_LABEL_LEFT);
	label_session = label_new(_("Load saved game:"), 300, 145, WIDGET_LABEL_LEFT);

	textfield_ip = text_field_new(config_get_str_value(CFG_NET_IP),
				      WIDGET_TEXTFIELD_FILTER_IP_OR_DOMAIN,
				      300, 180);

	textfield_port = text_field_new(config_get_str_value(CFG_NET_PORT),
					WIDGET_TEXTFIELD_FILTER_NUM, 300, 280);

	selectSession = select_new(300, 185, eventWidget);

	screen_register(screen_new("gameType", screen_startGameType, game_type_event,
			game_type_draw, stopScreenGameType));
}

int setSettingGameType(int status)
{
	if (status == NET_GAME_TYPE_NONE) {
		choiceGroup_set_status(check_none, TRUE);
		return 0;
	}

	if (status == NET_GAME_TYPE_SERVER) {
		choiceGroup_set_status(check_server, TRUE);
		return 0;
	}

	if (status == NET_GAME_TYPE_CLIENT) {
		choiceGroup_set_status(check_client, TRUE);
		return 0;
	}

	return -1;
}

int public_server_get_settingGameType()
{
	if (choice_group_get_status(check_none) == TRUE || choice_group_get_status(check_load_session) == TRUE) {
		return NET_GAME_TYPE_NONE;
	}

	if (choice_group_get_status(check_server) == TRUE) {
		return NET_GAME_TYPE_SERVER;
	}

	if (choice_group_get_status(check_client) == TRUE) {
		return NET_GAME_TYPE_CLIENT;
	}

	fatal("Unknown game type");

	return -1;
}

char *game_type_load_session()
{
	if (choice_group_get_status(check_load_session) == TRUE) {
		return select_get_item(selectSession);
	}

	return NULL;
}

int game_type_set_ip(char *address)
{
	char str[STR_SIZE];

	strcpy(str, address);
	text_field_set_text(textfield_ip, str);

	return 1;
}

char *public_server_get_settingIP()
{
	return text_field_get_text(textfield_ip);
}

int game_type_set_port(int port)
{
	char str[STR_SIZE];

	sprintf(str, "%d", port);
	text_field_set_text(textfield_port, str);

	return 0;
}

int public_server_get_settingPort()
{
	return atoi(text_field_get_text(textfield_port));
}

int public_server_get_settingProto()
{
	if (strstr(text_field_get_text(textfield_ip), ".") != NULL) {
		return PROTO_UDPv4;
	}

	if (strstr(text_field_get_text(textfield_ip), ":") != NULL) {
		return PROTO_UDPv6;
	}

	debug("Unknown version of IP protocol");

	return -1;
}

static void save_config()
{
	if (choice_group_get_status(check_none) == TRUE) {
		config_set_str_value(CFG_GAME_TYPE, "none");
	}

	if (choice_group_get_status(check_server) == TRUE) {
		config_set_str_value(CFG_GAME_TYPE, "server");
	}

	if (choice_group_get_status(check_client) == TRUE) {
		config_set_str_value(CFG_GAME_TYPE, "client");
	}

	if (choice_group_get_status(check_load_session) == TRUE) {
		config_set_str_value(CFG_GAME_TYPE, "session");
	}

	config_set_str_value(CFG_NET_IP, text_field_get_text(textfield_ip));
	config_set_str_value(CFG_NET_PORT, text_field_get_text(textfield_port));
}

void game_type_quit()
{
	save_config();

	wid_image_destroy(image_backgorund);

	label_destroy(label_none);
	label_destroy(label_server);
	label_destroy(label_client);
	label_destroy(label_load_session);

	label_destroy(label_ip);
	label_destroy(label_port);
	label_destroy(label_session);

	text_field_destroy(textfield_ip);
	text_field_destroy(textfield_port);

	select_destroy(selectSession);

	button_destroy(button_back);
	button_destroy(button_play);
	button_destroy(button_browser);

	choice_group_destroy(check_none);
	choice_group_destroy(check_server);
	choice_group_destroy(check_client);
	choice_group_destroy(check_load_session);

	list_destroy(listChoiceGroup);
}
