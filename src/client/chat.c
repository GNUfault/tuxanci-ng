
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include <SDL.h>

#include "main.h"
#include "list.h"
#include "myTimer.h"
#include "image.h"
#include "serverSendMsg.h"

#include "net_multiplayer.h"
#include "screen_world.h"
#include "proto.h"
#include "interface.h"
#include "chat.h"
#include "font.h"
#include "keyboardBuffer.h"
#include "hotKey.h"

static image_t *g_chat;

static list_t *listText;

static char line[STR_SIZE];

static int line_time, line_atime;
static int timeBlick;

static bool_t chat_active;
static bool_t receivedNewMsg;

static void hotkey_chat_enter();
static void hotkey_chat_enter();
static void eventChatDisable();

static void hotkey_chat_esc()
{
	//printf("*** hotkey_chat_esc\n");
	//unregisterHotKey(SDLK_ESCAPE);
	eventChatDisable();
}

static void hotkey_chat_enter()
{
	if( chat_active == TRUE )
	{
		return;
	}

	chat_active = TRUE;
	receivedNewMsg = FALSE;

	disableHotKey(SDLK_RETURN);
	disableHotKey(SDLK_p);
	registerHotKey(SDLK_ESCAPE, hotkey_chat_esc);

	enableKeyboardBuffer();
	clearKeyboardBuffer();
}

void initChat()
{
	g_chat = addImageData("chat.png", IMAGE_NO_ALPHA, "chat", IMAGE_GROUP_USER);

	listText = newList();
	strcpy(line, "");
	chat_active = FALSE;

	registerHotKey(SDLK_RETURN, hotkey_chat_enter);

	receivedNewMsg = FALSE;
	line_time = 0;
	line_atime = 0;
	timeBlick = 0;
}

void drawChat()
{
	char str[STR_SIZE];
	int i;

	if (chat_active == FALSE) {
		return;
	}

	drawImage(g_chat, CHAT_LOCATION_X, CHAT_LOCATION_Y,
			  0, 0, CHAT_SIZE_X, CHAT_SIZE_Y);

	for (i = 0; i < listText->count; i++) {
		char *s;

		s = (char *) listText->list[i];

		drawFontMaxSize(s, CHAT_LOCATION_X + 5, CHAT_LOCATION_Y + 5 + i * 20,
				   CHAT_SIZE_X - 10, 20, COLOR_WHITE);
	}

	strcpy(str, line);

	if (timeBlick > CHAT_TIME_BLICK_CURSOR / 2) {
		strcat(str, "\f");
	}

	timeBlick++;

	if (timeBlick == CHAT_TIME_BLICK_CURSOR) {
		timeBlick = 0;
	}

	drawFont(str, CHAT_LOCATION_X + 5, (CHAT_LOCATION_Y + 5) + (CHAT_MAX_LINES * 20), COLOR_WHITE);
}

static void processMessageKey(SDL_keysym keysym)
{
	int w, h;
	int len;
	//int i;

	if (line_atime < 100)
		line_atime++;

	len = strlen(line);
	getTextSize(line, &w, &h);

	if (w > CHAT_SIZE_X - 40) {
		return;
	}

	/* processing of backspace */
	if (keysym.sym == SDLK_BACKSPACE && len > 0) {
		line[len - 1] = '\0';
		return;
	}

	/* processing of printable chars but not letters */
	if ((keysym.sym >= SDLK_SPACE && keysym.sym <= SDLK_AT)
		|| (keysym.sym >= SDLK_LEFTBRACKET && keysym.sym <= SDLK_BACKQUOTE)) {
		/* here would be much better to put 'strcat' as 'line' is expected
		 * to be full of '0' up to its end
		 */
		line[len] = keysym.sym;
	}

	/* processing of letters */
	if (keysym.sym >= SDLK_a && keysym.sym <= SDLK_z) {
		char c = keysym.sym;
		if (keysym.mod == KMOD_SHIFT) {
			c = toupper(c);
		}
		/* here would be much better to put 'strcat' as 'line' is expected
		 * to be full of '0' up to its end
		 */
		line[len] = c;
	}

	return;
}

static void sendNewMessage()
{
	if (getNetTypeGame() == NET_GAME_TYPE_CLIENT) {
		proto_send_chat_client(line);
	}

	if (getNetTypeGame() == NET_GAME_TYPE_SERVER) {
		char out[STR_PROTO_SIZE];

		snprintf(out, STR_PROTO_SIZE, "chat %s:%s\n",
				 getControlTux(TUX_CONTROL_KEYBOARD_RIGHT)->name, line);

		proto_send_chat_server(PROTO_SEND_ALL, NULL, out);
	}
}

static void eventChatDisable()
{
	chat_active = FALSE;
	memset(line, '\0', STR_SIZE);

	enableHotKey(SDLK_RETURN);
	enableHotKey(SDLK_p);
	unregisterHotKey(SDLK_ESCAPE);

	disableKeyboardBuffer();
	clearKeyboardBuffer();
}

static void eventChatEnable()
{
	/* The chat window is displayed. It is needed to process all keys in the buffer
	 * but when ENTER is pressed the chat row is sent to the server
	 */

	while (isAnyKeyInKeyboardBuffer() == TRUE) {
		SDL_keysym key;
		key = popKeyFromKeyboardBuffer();

		if (key.sym == SDLK_RETURN) {
			if (strcmp(line, "") != 0) {
				/* the chat row is already written - it is needed to send it */
				sendNewMessage();
				memset(line, '\0', STR_SIZE);
				continue;		/* continue with other keys */
			} else {
				/* the chat row is empty - it is needed to turn off the chat window */
				//eventChatDisable();
				/* turn off key catching into the buffer and clear the buffer */
				//disableKeyboardBuffer();
				//clearKeyboardBuffer();
			}
		}

		processMessageKey(key);
	}
}

/**
 * Processing of chat 'events'
 */
void eventChat()
{
	if (isChatActive()) {
		eventChatEnable();
	}
}

bool_t isChatActive()
{
	return chat_active;
}

bool_t isRecivedNewMsg()
{
	return receivedNewMsg;
}

void addToChat(char *s)
{
	addList(listText, strdup(s));

	if (listText->count > CHAT_MAX_LINES) {
		delListItem(listText, 0, free);
	}

	if (chat_active == FALSE) {
		receivedNewMsg = TRUE;
	}
}

void quitChat()
{
	assert(listText != NULL);

	//unregisterHotKey(SDLK_RETURN);
	destroyListItem(listText, free);
}
