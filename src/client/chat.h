
#ifndef CHAT_H

#    define CHAT_H

#    define CHAT_MAX_LINES		10
#    define CHAT_TIME_MULTIPLE 	10
#    define CHAT_TIME_BLICK_CURSOR	20
#    define CHAT_LINE_WIDTH 	320

#    define CHAT_SIZE_X		320
#    define CHAT_SIZE_Y		240

#    define CHAT_LOCATION_X		(WINDOW_SIZE_X/2 - CHAT_SIZE_X/2)
#    define CHAT_LOCATION_Y		(WINDOW_SIZE_Y/2 - CHAT_SIZE_Y/2)

extern void chat_init();
extern void chat_draw();
extern void chat_event();
extern void chat_add(char *s);
extern bool_t chat_is_active();
extern bool_t chat_is_recived_new_msg();
extern void chat_quit();

#endif
