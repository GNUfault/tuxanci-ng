
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

static SDL_Surface *g_chat;

static list_t *listText;

static char line[STR_SIZE];

static int line_time, line_atime;
static int timeBlick;

static bool_t chat_active;
static bool_t revicedNewMsg;

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
	int len;
	int w, h;
	int i;

	if (line_atime < 100)
		line_atime ++;

	mapa = SDL_GetKeyState(NULL);
	len = strlen(line);

	getTextSize(line, &w, &h);
	
	// mazanie posledneho klavesu
	if( mapa[SDLK_BACKSPACE] == SDL_PRESSED )
	{
		if( len > 0 )
		{
			line_time = 0;
			line[len-1]='\0';
		}

		return;
	}

	if( w > CHAT_LINE_WIDTH-40 )
	{
		return;
	}

	// klavesy abecedy
	for(i=SDLK_SPACE /*SDLK_a*/;i<=SDLK_z;i++)
	{
		//if(width<TEXTFIELD_SIZE_X-20 && mapa[i]==SDL_PRESSED)
		if( mapa[i] == SDL_PRESSED && len < STR_SIZE )
		{
			if (i == SDLK_SPACE)
			{
				strcat( line, " " );
				return;
			}

			const char *c = (const char *) SDL_GetKeyName(i);

			if (len) {
				if (line[len-1] == *c) {
					line_time ++;

					if ( line_time < CHAT_TIME_MULTIPLE) {
						if ( line_atime < 3)
							return;
					}
				} else
					line_time = 0;
			}

			line_atime = 0;

			strcat( line, c );
			getTextSize(line, &w, &h);
			
			if( mapa[SDLK_LSHIFT] == SDL_PRESSED ||
			    mapa[SDLK_RSHIFT] == SDL_PRESSED)
			{
				line[ strlen( line ) - 1 ] -= 32;
				return;
			}
		}
	}

	// aby ve jmene bulanka fungovaly take cislice alfanumericke klavesnice
	for( i = SDLK_0 ; i <= SDLK_9 ; i++ )
	{
		if( mapa[i] == SDL_PRESSED && len < STR_SIZE )
		{
			const char *c = (const char *) SDL_GetKeyName(i);

			if (len) {
				if (line[len-1] == *c) {
					line_time ++;

					if (line_time < CHAT_TIME_MULTIPLE) {
						if (line_atime < 3)
							return;
					}
				} else
					line_time = 0;
			}

			line_atime = 0;

			strcat( line, c );
			return;
		}
	}
	
	// aby ve jmene bulanka fungovaly take cislice napsane na numericke klavesnici
	for( i=SDLK_KP0 ; i<=SDLK_KP9 ; i++ )
	{
		if( mapa[i] == SDL_PRESSED && len < STR_SIZE )
		{
			const char *c = (const char *) SDL_GetKeyName(i)+1;

			if (len) {
				if (line[len-1] == *c) {
					line_time ++;

					if (line_time < CHAT_TIME_MULTIPLE) {
						if (line_atime < 3)
							return;
					}
				} else
					line_time = 0;
			}

			line_atime = 0;

			strncat( line, c, 1 ); // napr. "[4]"
			return;
		}
	}
}

void eventChat()
{
	my_time_t currentTime;
	Uint8 *mapa;
	
	mapa = SDL_GetKeyState(NULL);

	if( mapa[SDLK_RETURN] == SDL_PRESSED && chat_active == FALSE )
	{
		revicedNewMsg = FALSE;
		chat_active = TRUE;
	}

	if( mapa[SDLK_ESCAPE] == SDL_PRESSED && chat_active == TRUE )
	{
		chat_active = FALSE;
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

		strcpy(line, "");
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
