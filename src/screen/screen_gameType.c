
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "main.h"
#include "list.h"
#include "director.h"
#include "homeDirector.h"
#include "net_multiplayer.h"
#include "tux.h"

#include "language.h"
#include "interface.h"
#include "screen.h"
#include "image.h"

#ifndef NO_SOUND
#include "music.h"
#endif

#include "screen_gameType.h"
#include "screen_setting.h"

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

	director = loadDirector(getHomeDirector());

	removeAllFromWidgetSelect(p);

	if( director == NULL )
	{
		return;
	}

	for( i = 0 ; i < director->list->count ; i++ )
	{
		char *line;

		line = director->list->list[i];

		if( strstr(line, ".sav") != NULL )
		{
			addToWidgetSelect(p, line);
		}
	}

	destroyDirector(director);
}

void startScreenGameType()
{
#ifndef NO_SOUND
	playMusic("menu", MUSIC_GROUP_BASE);
#endif

	setWidgetChoiceStatus(check_none, TRUE);
	setWidgetChoiceStatus(check_server, FALSE);
	setWidgetChoiceStatus(check_client, FALSE);
	setWidgetChoiceStatus(check_load_session, FALSE);

	loadSession(selectSession);
}

void drawScreenGameType()
{
	int i;

	drawWidgetImage(image_backgorund);
	
	for( i = 0 ; i < listChoiceGroup->count ; i++ )
	{
		widget_t *this;

		this = (widget_t *)listChoiceGroup->list[i];
		drawWidgetChoicegroup(this);
	}

	drawWidgetLabel(label_none);
	drawWidgetLabel(label_server);
	drawWidgetLabel(label_client);
	drawWidgetLabel(label_load_session);

	if( getWidgetChoiceStatus(check_server) == TRUE || getWidgetChoiceStatus(check_client) == TRUE )
	{
		drawWidgetLabel(label_port);
		drawWidgetTextfield(textfield_port);
		drawWidgetLabel(label_ip);
		drawWidgetTextfield(textfield_ip);
	}

	if( getWidgetChoiceStatus(check_load_session) == TRUE )
	{
		drawWidgetLabel(label_session);
		drawWidgetSelect(selectSession);
	}

#if 0
	if( check_server->status == TRUE )
	{
		setSettingIP (getParamElse ("--ip", "127.0.0.1"));
		setSettingPort (atoi (getParamElse("--port", "2200")));
	}
#endif

	if( getWidgetChoiceStatus(check_client) == TRUE )
		drawWidgetButton(button_browser);
	
	drawWidgetButton(button_back);
	drawWidgetButton(button_play);

}

void eventScreenGameType()
{
	int i;

	for( i = 0 ; i < listChoiceGroup->count ; i++ )
	{
		widget_t *this;

		this = (widget_t *)listChoiceGroup->list[i];
		eventWidgetChoicegroup(this);
	}

	if( getWidgetChoiceStatus(check_server) == TRUE || getWidgetChoiceStatus(check_client) == TRUE )
	{
		eventWidgetTextfield(textfield_ip);
		eventWidgetTextfield(textfield_port);
	}

	if( getWidgetChoiceStatus(check_load_session) == TRUE )
	{
		eventWidgetSelect(selectSession);
	}

	eventWidgetButton(button_back);
	eventWidgetButton(button_play);

	if( getWidgetChoiceStatus(check_client) == TRUE )
		eventWidgetButton(button_browser);
}

void stopScreenGameType()
{
}

static void eventWidget(void *p)
{
	widget_t *button;
	
	button = (widget_t *)(p);

	if( button == button_back )
	{
		setScreen("mainMenu");
	}

	if( button == button_play )
	{
		char *str;

		str = getGemeTypeLoadSession();

		if( str != NULL )
		{
			setScreen("world");
			return;
		}

		if( getSettingGameType() == NET_GAME_TYPE_CLIENT )
		{
			setScreen("world");
		}
		else
		{
			setScreen("chiceArena");
		}
	}

	if( button == button_browser )
	{
		setScreen("browser");
	}
}

void initScreenGameType()
{
	image_t *image;

	image = getImage(IMAGE_GROUP_BASE, "screen_main");
	image_backgorund  = newWidgetImage(0, 0, image);

	button_back = newWidgetButton(_("back"), 100, WINDOW_SIZE_Y-100, eventWidget);
	button_play = newWidgetButton(_("play"), WINDOW_SIZE_X-200, WINDOW_SIZE_Y-100, eventWidget);
	button_browser = newWidgetButton(_("browser"), 300, 345, eventWidget);

	listChoiceGroup = newList();
	check_none = newWidgetChoicegroup(100, 150, FALSE, listChoiceGroup, eventWidget);
	check_server = newWidgetChoicegroup(100, 200, FALSE, listChoiceGroup, eventWidget);
	check_client = newWidgetChoicegroup(100, 250, FALSE, listChoiceGroup, eventWidget);
	check_load_session = newWidgetChoicegroup(100, 300, FALSE, listChoiceGroup, eventWidget);

	if( isParamFlag("--server") )
	{
		setWidgetChoiceStatus(check_server, TRUE);
	}
	else if( isParamFlag("--client") )
	{
		setWidgetChoiceStatus(check_client, TRUE);
	}
	else
	{
		setWidgetChoiceStatus(check_none, TRUE);
	}

	label_none = newWidgetLabel(_("Local game"), 130, 145, WIDGET_LABEL_LEFT);
	label_server = newWidgetLabel(_("Set up a server"), 130, 195, WIDGET_LABEL_LEFT);
	label_client = newWidgetLabel(_("Network game"), 130, 245, WIDGET_LABEL_LEFT);
	label_load_session = newWidgetLabel("load session", 130, 295, WIDGET_LABEL_LEFT);

	label_ip = newWidgetLabel(_("IP"), 300, 145, WIDGET_LABEL_LEFT);
	label_port = newWidgetLabel(_("port"), 300, 245, WIDGET_LABEL_LEFT);
	label_session = newWidgetLabel("load session :", 300, 145, WIDGET_LABEL_LEFT);

	textfield_ip = newWidgetTextfield(
		getParamElse("--ip", "127.0.0.1"),
		WIDGET_TEXTFIELD_FILTER_IP_OR_DOMAIN,
		300, 180);
	
	textfield_port = newWidgetTextfield(
		getParamElse("--port", "2200"),
		WIDGET_TEXTFIELD_FILTER_NUM,
		300, 280);

	selectSession = newWidgetSelect(300, 185, eventWidget);

	registerScreen( newScreen("gameType", startScreenGameType, eventScreenGameType,
		drawScreenGameType, stopScreenGameType) );
}

int setSettingGameType(int status)
{
	if( status == NET_GAME_TYPE_NONE )
	{
		setWidgetChoiceStatus(check_none, TRUE);
		return 0;
	}

	if( status == NET_GAME_TYPE_SERVER )
	{
		setWidgetChoiceStatus(check_server, TRUE);
		return 0;
	}

	if( status == NET_GAME_TYPE_CLIENT )
	{
		setWidgetChoiceStatus(check_client, TRUE);
		return 0;
	}

	return -1;
}

int getSettingGameType()
{
	if( getWidgetChoiceStatus(check_none) == TRUE  ||
	    getWidgetChoiceStatus(check_load_session) == TRUE )
	{
		return NET_GAME_TYPE_NONE;
	}

	if( getWidgetChoiceStatus(check_server) == TRUE )
	{
		return NET_GAME_TYPE_SERVER;
	}

	if( getWidgetChoiceStatus(check_client) == TRUE )
	{
		return NET_GAME_TYPE_CLIENT;
	}

	assert( ! _("Unknown game type!") );

	return -1;
}

char* getGemeTypeLoadSession()
{
	if( getWidgetChoiceStatus(check_load_session) == TRUE )
	{
		return getWidgetSelectItem(selectSession);
	}

	return NULL;
}

int setSettingIP(char *address)
{
	char str[STR_SIZE];

	strcpy (str, address);
	setWidgetTextFiledText(textfield_ip, str);

	return 1;
}

char* getSettingIP()
{
	return getTextFromWidgetTextfield(textfield_ip);
}

int setSettingPort(int port)
{
	char str[STR_SIZE];

	sprintf (str, "%d", port);
	setWidgetTextFiledText(textfield_port, str);

	return 0;
}

int getSettingPort()
{
	return atoi( getTextFromWidgetTextfield(textfield_port) );
}

int getSettingProto()
{
	if( strstr(getTextFromWidgetTextfield(textfield_ip), ".") != NULL )
	{
		return PROTO_UDPv4;
	}

	if( strstr(getTextFromWidgetTextfield(textfield_ip), ":") != NULL )
	{
		return PROTO_UDPv6;
	}
#ifdef DEBUG
	printf(_("Unknown IP protocol!\n"));
#endif
	return -1;
}

void quitScreenGameType()
{
	destroyWidgetImage(image_backgorund);

	destroyWidgetLabel(label_none);
	destroyWidgetLabel(label_server);
	destroyWidgetLabel(label_client);
	destroyWidgetLabel(label_load_session);

	destroyWidgetLabel(label_ip);
	destroyWidgetLabel(label_port);
	destroyWidgetLabel(label_session);

	destroyWidgetTextfield(textfield_ip);
	destroyWidgetTextfield(textfield_port);

	destroyWidgetSelect(selectSession);

	destroyWidgetButton(button_back);
	destroyWidgetButton(button_play);
	destroyWidgetButton(button_browser);

	destroyWidgetChoicegroup(check_none);
	destroyWidgetChoicegroup(check_server);
	destroyWidgetChoicegroup(check_client);
	destroyWidgetChoicegroup(check_load_session);
	
	destroyList(listChoiceGroup);
}

