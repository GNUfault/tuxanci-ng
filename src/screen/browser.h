
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


extern void browser_start();
extern void browser_draw();
extern void browser_event();
extern void browser_stop();
extern void browser_init();
extern void browser_quit();

#endif
