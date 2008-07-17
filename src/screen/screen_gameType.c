
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "main.h"
#include "list.h"
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

#include "widget_label.h"
#include "widget_button.h"
#include "widget_image.h"
#include "widget_textfield.h"
#include "widget_check.h"

static widget_image_t *image_backgorund;

static widget_button_t *button_back;
static widget_button_t *button_play;
static widget_button_t *button_browser;

static widget_label_t *label_none;
static widget_label_t *label_server;
static widget_label_t *label_client;

static widget_label_t *label_ip;
static widget_label_t *label_port;

static widget_check_t *check_none;
static widget_check_t *check_server;
static widget_check_t *check_client;

static widget_textfield_t *textfield_ip;
static widget_textfield_t *textfield_port;

void startScreenGameType()
{
#ifndef NO_SOUND
	playMusic("menu", MUSIC_GROUP_BASE);
#endif

	check_none->status = TRUE;
	check_server->status = FALSE;
	check_client->status = FALSE;
}

void drawScreenGameType()
{
	drawWidgetImage(image_backgorund);
	
	drawWidgetLabel(label_none);
	drawWidgetLabel(label_server);
	drawWidgetLabel(label_client);

	if( check_none->status == FALSE )
	{
		drawWidgetLabel(label_port);
		drawWidgetTextfield(textfield_port);
		drawWidgetLabel(label_ip);
		drawWidgetTextfield(textfield_ip);
	}

#if 0
	if( check_server->status == TRUE )
	{
		setSettingIP (getParamElse ("--ip", "127.0.0.1"));
		setSettingPort (atoi (getParamElse("--port", "2200")));
	}
#endif

	if( check_client->status == TRUE )
		drawWidgetButton(button_browser);

	drawWidgetCheck(check_none);
	drawWidgetCheck(check_server);
	drawWidgetCheck(check_client);

	drawWidgetButton(button_back);
	drawWidgetButton(button_play);

}

void eventScreenGameType()
{
	eventWidgetCheck(check_none);
	eventWidgetCheck(check_server);
	eventWidgetCheck(check_client);

	eventWidgetTextfield(textfield_ip);
	eventWidgetTextfield(textfield_port);

	eventWidgetButton(button_back);
	eventWidgetButton(button_play);

	if( check_client->status == TRUE )
		eventWidgetButton(button_browser);
}

void stopScreenGameType()
{
}

static void eventWidget(void *p)
{
	widget_button_t *button;
	widget_check_t *check;
	
	button = (widget_button_t *)(p);
	check = (widget_check_t *)(p);

	if( check == check_none )
	{
		check_none->status = TRUE;
		check_server->status = FALSE;
		check_client->status = FALSE;
		return;
	}

	if( check == check_server )
	{
		check_none->status = FALSE;
		check_server->status = TRUE;
		check_client->status = FALSE;
		return;
	}

	if( check == check_client )
	{
		check_none->status = FALSE;
		check_server->status = FALSE;
		check_client->status = TRUE;
		return;
	}

	if( button == button_back )
	{
		setScreen("mainMenu");
	}

	if( button == button_play )
	{
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

	button_back = newWidgetButton(getMyText("BACK"), 100, WINDOW_SIZE_Y-100, eventWidget);
	button_play = newWidgetButton(getMyText("PLAY"), WINDOW_SIZE_X-200, WINDOW_SIZE_Y-100, eventWidget);
	button_browser = newWidgetButton(getMyText("BROWSER"), 300, 345, eventWidget);

	check_none = newWidgetCheck(100, 150, FALSE, eventWidget);
	check_server = newWidgetCheck(100, 200, FALSE, eventWidget);
	check_client = newWidgetCheck(100, 250, FALSE, eventWidget);

	if( isParamFlag("--server") )
	{
		check_server->status = TRUE;
	}
	else if( isParamFlag("--client") )
	{
		check_client->status = TRUE;
	}
	else
	{
		check_none->status = TRUE;
	}

	label_none = newWidgetLabel(getMyText("NONE"), 130, 145, WIDGET_LABEL_LEFT);
	label_server = newWidgetLabel(getMyText("SERVER"), 130, 195, WIDGET_LABEL_LEFT);
	label_client = newWidgetLabel(getMyText("CLIENT"), 130, 245, WIDGET_LABEL_LEFT);

	label_ip = newWidgetLabel(getMyText("IP_ADDR"), 300, 145, WIDGET_LABEL_LEFT);
	label_port = newWidgetLabel(getMyText("NET_PORT"), 300, 245, WIDGET_LABEL_LEFT);

	textfield_ip = newWidgetTextfield(
		getParamElse("--ip", "127.0.0.1"),
		WIDGET_TEXTFIELD_FILTER_IP_OR_DOMAIN,
		300, 180);
	
	textfield_port = newWidgetTextfield(
		getParamElse("--port", "2200"),
		WIDGET_TEXTFIELD_FILTER_NUM,
		300, 280);

	registerScreen( newScreen("gameType", startScreenGameType, eventScreenGameType,
		drawScreenGameType, stopScreenGameType) );
}

int setSettingGameType(int status)
{
	check_none->status = status;

	return 0;
}

int getSettingGameType()
{
	if( check_none->status == TRUE )
	{
		return NET_GAME_TYPE_NONE;
	}

	if( check_server->status == TRUE )
	{
		return NET_GAME_TYPE_SERVER;
	}

	if( check_client->status == TRUE )
	{
		return NET_GAME_TYPE_CLIENT;
	}

	assert( ! "Chyba pri zistvani type  hry !" );

	return -1;
}

int setSettingIP(char *address)
{
	strcpy (textfield_ip->text, address);

	return 1;
}

char* getSettingIP()
{
	return textfield_ip->text;
}

int setSettingPort(int port)
{
	sprintf (textfield_port->text, "%d", port);

	return 0;
}

int getSettingPort()
{
	return atoi( textfield_port->text );
}

int getSettingProto()
{
	if( strstr(textfield_ip->text, ".") != NULL )
	{
		return PROTO_UDPv4;
	}

	if( strstr(textfield_ip->text, ":") != NULL )
	{
		return PROTO_UDPv6;
	}

	printf("IP protokol unknown !\n");
	return -1;
}

void quitScreenGameType()
{
	destroyWidgetImage(image_backgorund);

	destroyWidgetLabel(label_none);
	destroyWidgetLabel(label_server);
	destroyWidgetLabel(label_client);

	destroyWidgetLabel(label_ip);
	destroyWidgetLabel(label_port);

	destroyWidgetTextfield(textfield_ip);
	destroyWidgetTextfield(textfield_port);

	destroyWidgetButton(button_back);
	destroyWidgetButton(button_play);
	destroyWidgetButton(button_browser);

	destroyWidgetCheck(check_none);
	destroyWidgetCheck(check_server);
	destroyWidgetCheck(check_client);
}

