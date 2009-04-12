
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

#ifndef NO_SOUND
#    include "music.h"
#endif

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

static void loadSession(widget_t * p)
{
	director_t *director;
	int i;

	director = director_load(homeDirector_get());

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
#endif

	choiceGroup_set_status(check_none, TRUE);
	choiceGroup_set_status(check_server, FALSE);
	choiceGroup_set_status(check_client, FALSE);
	choiceGroup_set_status(check_load_session, FALSE);

	loadSession(selectSession);

	hotKey_register(SDLK_ESCAPE, hotkey_escape);
}

void gameType_draw()
{
	int i;

	wid_image_draw(image_backgorund);

	for (i = 0; i < listChoiceGroup->count; i++) {
		widget_t *this;

		this = (widget_t *) listChoiceGroup->list[i];
		choiceGroup_draw(this);
	}

	label_draw(label_none);
	label_draw(label_server);
	label_draw(label_client);
	label_draw(label_load_session);

	if (choiceGroup_get_status(check_server) == TRUE || choiceGroup_get_status(check_client) == TRUE) {
		label_draw(label_port);
		textField_draw(textfield_port);
		label_draw(label_ip);
		textField_draw(textfield_ip);
	}

	if (choiceGroup_get_status(check_load_session) == TRUE) {
		label_draw(label_session);
		select_draw(selectSession);
	}
#if 0
	if (check_server->status == TRUE) {
		gameType_set_ip(getParamElse("--ip", "127.0.0.1"));
		gameType_set_port(atoi(getParamElse("--port", "2200")));
	}
#endif

	if (choiceGroup_get_status(check_client) == TRUE)
		button_draw(button_browser);

	button_draw(button_back);
	button_draw(button_play);

}

void gameType_event()
{
	int i;

	for (i = 0; i < listChoiceGroup->count; i++) {
		widget_t *this;

		this = (widget_t *) listChoiceGroup->list[i];
		choiceGroup_event(this);
	}

	if (choiceGroup_get_status(check_server) == TRUE || choiceGroup_get_status(check_client) == TRUE) {
		textField_event(textfield_ip);
		textField_event(textfield_port);
	}

	if (choiceGroup_get_status(check_load_session) == TRUE) {
		select_event(selectSession);
	}

	button_event(button_back);
	button_event(button_play);

	if (choiceGroup_get_status(check_client) == TRUE)
		button_event(button_browser);
}

void stopScreenGameType()
{
	unhotKey_register(SDLK_ESCAPE);
}

static void eventWidget(void *p)
{
	widget_t *button;

	button = (widget_t *) (p);

	if (button == button_back) {
		screen_set("mainMenu");
	}

	if (button == button_play) {
		char *str;

		str = gameType_load_session();

		if (str != NULL) {
			screen_set("world");
			return;
		}

		if (publicServer_get_settingGameType() == NET_GAME_TYPE_CLIENT) {
			screen_set("downArena");
			//screen_set("world");
		} else {
			screen_set("chiceArena");
		}
	}

	if (button == button_browser) {
		screen_set("browser");
	}
}

void gameType_init()
{
	image_t *image;

	image = image_get(IMAGE_GROUP_BASE, "screen_main");
	image_backgorund = wid_image_new(0, 0, image);

	button_back = button_new(_("back"), 100, WINDOW_SIZE_Y - 100, eventWidget);
	button_play = button_new(_("play"), WINDOW_SIZE_X - 200, WINDOW_SIZE_Y - 100, eventWidget);
	button_browser = button_new(_("browser"), 300, 345, eventWidget);

	listChoiceGroup = list_new();
	check_none = choiceGroup_new(100, 150, FALSE, listChoiceGroup, eventWidget);
	check_server = choiceGroup_new(100, 200, FALSE, listChoiceGroup, eventWidget);
	check_client = choiceGroup_new(100, 250, FALSE, listChoiceGroup, eventWidget);
	check_load_session = choiceGroup_new(100, 300, FALSE, listChoiceGroup, eventWidget);

	if (isParamFlag("--server")) {
		choiceGroup_set_status(check_server, TRUE);
	} else if (isParamFlag("--client")) {
		choiceGroup_set_status(check_client, TRUE);
	} else {
		choiceGroup_set_status(check_none, TRUE);
	}

	label_none = label_new(_("Local game"), 130, 145, WIDGET_LABEL_LEFT);
	label_server = label_new(_("Set up a server"), 130, 195, WIDGET_LABEL_LEFT);
	label_client = label_new(_("Network game"), 130, 245, WIDGET_LABEL_LEFT);
	label_load_session = label_new("load session", 130, 295, WIDGET_LABEL_LEFT);

	label_ip = label_new(_("IP"), 300, 145, WIDGET_LABEL_LEFT);
	label_port = label_new(_("port"), 300, 245, WIDGET_LABEL_LEFT);
	label_session = label_new("load session :", 300, 145, WIDGET_LABEL_LEFT);

	textfield_ip = textField_new(getParamElse("--ip", "127.0.0.1"),
					  WIDGET_TEXTFIELD_FILTER_IP_OR_DOMAIN,
					  300, 180);

	textfield_port = textField_new(getParamElse("--port", "6800"),
					    WIDGET_TEXTFIELD_FILTER_NUM, 300, 280);

	selectSession = select_new(300, 185, eventWidget);

	screen_register( screen_new("gameType", screen_startGameType, gameType_event,
			gameType_draw, stopScreenGameType));
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

int publicServer_get_settingGameType()
{
	if (choiceGroup_get_status(check_none) == TRUE || choiceGroup_get_status(check_load_session) == TRUE) {
		return NET_GAME_TYPE_NONE;
	}

	if (choiceGroup_get_status(check_server) == TRUE) {
		return NET_GAME_TYPE_SERVER;
	}

	if (choiceGroup_get_status(check_client) == TRUE) {
		return NET_GAME_TYPE_CLIENT;
	}

	assert(!_("Unknown game type!"));

	return -1;
}

char *gameType_load_session()
{
	if (choiceGroup_get_status(check_load_session) == TRUE) {
		return select_get_item(selectSession);
	}

	return NULL;
}

int gameType_set_ip(char *address)
{
	char str[STR_SIZE];

	strcpy(str, address);
	textField_set_text(textfield_ip, str);

	return 1;
}

char *publicServer_get_settingIP()
{
	return textField_get_text(textfield_ip);
}

int gameType_set_port(int port)
{
	char str[STR_SIZE];

	sprintf(str, "%d", port);
	textField_set_text(textfield_port, str);

	return 0;
}

int publicServer_get_settingPort()
{
	return atoi(textField_get_text(textfield_port));
}

int publicServer_get_settingProto()
{
	if (strstr(textField_get_text(textfield_ip), ".") != NULL) {
		return PROTO_UDPv4;
	}

	if (strstr(textField_get_text(textfield_ip), ":") != NULL) {
		return PROTO_UDPv6;
	}

	DEBUG_MSG(_("Unknown IP protocol!\n"));

	return -1;
}

void gameType_quit()
{
	wid_image_destroy(image_backgorund);

	label_destroy(label_none);
	label_destroy(label_server);
	label_destroy(label_client);
	label_destroy(label_load_session);

	label_destroy(label_ip);
	label_destroy(label_port);
	label_destroy(label_session);

	textField_destroy(textfield_ip);
	textField_destroy(textfield_port);

	select_destroy(selectSession);

	button_destroy(button_back);
	button_destroy(button_play);
	button_destroy(button_browser);

	choiceGroup_destroy(check_none);
	choiceGroup_destroy(check_server);
	choiceGroup_destroy(check_client);
	choiceGroup_destroy(check_load_session);

	list_destroy(listChoiceGroup);
}
