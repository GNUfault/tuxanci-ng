
#ifndef CHAT_H

#define CHAT_H

#define CHAT_MAX_LINES		10
#define CHAT_TIME_MULTIPLE 	10
#define CHAT_TIME_BLICK_CURSOR	20
#define CHAT_LINE_WIDTH 	320

#define CHAT_SIZE_X		320
#define CHAT_SIZE_Y		240

#define CHAT_LOCATION_X		(WINDOW_SIZE_X/2 - CHAT_SIZE_X/2)
#define CHAT_LOCATION_Y		(WINDOW_SIZE_Y/2 - CHAT_SIZE_Y/2)

#define CHAT_LAST_ENTER_INTERVAL	50

extern void initChat();
extern void drawChat();
extern void eventChat();
extern void addToChat(char *s);
extern bool_t isChatActive();
extern bool_t isRecivedNewMsg();
extern void quitChat();

#endif
