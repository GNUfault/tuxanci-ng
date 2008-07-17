
#include <stdlib.h>
#include <assert.h>

#include "main.h"
#include "list.h"
#include "myTimer.h"
#include "image.h"

#include "net_multiplayer.h"
#include "screen_world.h"
#include "proto.h"
#include "interface.h"
#include "chat.h"
#include "font.h"

static image_t *g_chat;

static list_t *listText;

static char line[STR_SIZE];

static int line_time, line_atime;
static int timeBlick;

static bool_t chat_active;
static bool_t revicedNewMsg;

static my_time_t lastActive;

void initChat()
{
	g_chat = addImageData("chat.png", IMAGE_NO_ALPHA, "chat", IMAGE_GROUP_USER);

	listText = newList();
	strcpy(line, "");
	chat_active = FALSE;
	revicedNewMsg = FALSE;
	line_time = 0;
	line_atime = 0;
	timeBlick = 0;
	lastActive = 0;
}

void drawChat()
{
	char str[STR_SIZE];
	int i;

	if( chat_active == FALSE )
	{
		return;
	}

	drawImage(g_chat,
		CHAT_LOCATION_X, CHAT_LOCATION_Y,
		0, 0, CHAT_SIZE_X, CHAT_SIZE_Y);

	for( i = 0 ; i < listText->count ; i++ )
	{
		char *s;

		s = (char *)listText->list[i];
		
		drawFontMaxSize(s,
			CHAT_LOCATION_X+5, CHAT_LOCATION_Y+5 + i*20,
			CHAT_SIZE_X-10, 20, COLOR_WHITE);
	}

	strcpy(str, line);

	if( timeBlick > CHAT_TIME_BLICK_CURSOR/2 )
	{
		strcat(str, "\f");
	}

	timeBlick++;

	if( timeBlick == CHAT_TIME_BLICK_CURSOR )
	{
		timeBlick = 0;
	}

	drawFont(str, CHAT_LOCATION_X+5, CHAT_LOCATION_Y+5 + CHAT_MAX_LINES*20, COLOR_WHITE);
}

static void readKey()
{
	Uint8 *mapa;
	int w, h;
	int len;
	int i;

	const int len_shift_map = 20;

	char shift_map[] =
	{
		'-', '_',
		'=', '+',
		'[', '{',
		']', '}',
		';', ':',
		'/', '\"',
		'\\', '|',
		',', '<',
		'.', '>',
		'/', '?'
	};

	if (line_atime < 100)
		line_atime ++;

	mapa = SDL_GetKeyState(NULL);
	len = strlen(line);
	getTextSize(line, &w, &h);

	for( i = SDLK_FIRST ; i <= SDLK_F15 ; i++ )
	{
		if( mapa[i] == SDL_PRESSED && len < STR_SIZE )
		{
			char *name = (char *) SDL_GetKeyName(i);
			char c = '\0';

			if( name == NULL )continue;

			if( strlen(name) == 1 )c = name[0];
			if( strlen(name) == 3 )c = name[1];
			if( strcmp(name, "space") == 0 )c = ' ';

			//printf("name %s\n", name);

			if( strcmp(name, "backspace") == 0 && len > 0 )
			{
				line_time = 0;
				line[len-1]='\0';
				return;
			}

			if( c == '\0' || w > CHAT_SIZE_X-40 )
			{
				continue;
			}

			if( len > 0 )
			{
				if( line[len-1] == c )
				{
					line_time++;

					if( line_time < CHAT_TIME_MULTIPLE )
					{
						if( line_atime < 3 )return;
					}
				}
				else
				{
					line_time = 0;
				}
			}

			line_atime = 0;

			line[len] = c;

			if( mapa[SDLK_LSHIFT] == SDL_PRESSED ||
			    mapa[SDLK_RSHIFT] == SDL_PRESSED)
			{
				int i;

				for( i = 0 ; i < len_shift_map ; i+= 2 )
				{
					if( c == shift_map[i] )
					{
						line[len] = shift_map[i+1];
					}
				}

				return;
			}

			if( mapa[SDLK_LSHIFT] == SDL_PRESSED ||
			    mapa[SDLK_RSHIFT] == SDL_PRESSED)
			{
				line[len] -= 32;
			}

			return;
		}
	}
}

static void switchChatActive()
{
	if( chat_active == TRUE )
	{
		chat_active = FALSE;
	}
	else
	{
		chat_active = TRUE;
	}
}

void eventChat()
{
	Uint8 *mapa;
	my_time_t currentTime;

	mapa = SDL_GetKeyState(NULL);
	currentTime = getMyTime();
	
	if( mapa[SDLK_RETURN] == SDL_PRESSED &&
	    strcmp(line, "") == 0 &&
	    currentTime - lastActive > CHAT_LAST_ENTER_INTERVAL )
	{
		lastActive = getMyTime();
		revicedNewMsg = FALSE;
		switchChatActive();
		memset(line, '\0', STR_SIZE);
		return;
	}

	if( chat_active == FALSE )
	{
		return;
	}

	if( mapa[SDLK_RETURN] == SDL_PRESSED && strcmp(line, "") != 0 )
	{
		if( getNetTypeGame() == NET_GAME_TYPE_CLIENT )
		{
			proto_send_chat_client(line);
		}

		if( getNetTypeGame() == NET_GAME_TYPE_SERVER )
		{
			char out[STR_PROTO_SIZE];
			
			snprintf(out, STR_PROTO_SIZE, "chat %s:%s\n",
				getControlTux(TUX_CONTROL_KEYBOARD_RIGHT)->name, line);
			
			proto_send_chat_server(PROTO_SEND_ALL, NULL, out);
		}

		memset(line, '\0', STR_SIZE);
		return;
	}

	readKey();
}

bool_t isChatActive()
{
	return chat_active;
}

bool_t isRecivedNewMsg()
{
	return revicedNewMsg;
}

void addToChat(char *s)
{
	addList(listText, strdup(s) );
	
	if( listText->count > CHAT_MAX_LINES )
	{
		delListItem(listText, 0, free);
	}

	if( chat_active == FALSE )
	{
		revicedNewMsg = TRUE;
	}
}

void quitChat()
{
	assert( listText != NULL );
	destroyListItem(listText, free);
}
