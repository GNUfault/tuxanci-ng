
#ifndef SCREEN_BROWSER_H

#    define SCREEN_BROWSER_H

typedef struct server_context {
	struct server_context *next, *prev;

	unsigned char state;

	unsigned char ping;

	unsigned ip;
	unsigned port;

	char *name;

	char *version;
	unsigned char clients;
	unsigned char maxclients;
	unsigned uptime;
	char *arena;
} server_t;


extern void startScreenBrowser();
extern void drawScreenBrowser();
extern void eventScreenBrowser();
extern void stopScreenBrowser();
extern void initScreenBrowser();
extern void quitScreenBrowser();

#endif
