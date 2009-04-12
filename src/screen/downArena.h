
#ifndef SCREEN_DOWN_ARENA_H

#    define SCREEN_DOWN_ARENA_H

#    define DOWN_SERVER_STATUS_OK		0
#    define DOWN_SERVER_STATUS_ZOMBIE	1

#    define	DOWN_ARENA_BUFFER_SIZE	4096

#    define DOWN_ARENA_MAX_SEND_MSG_STATUS	10
#    define DOWN_ARENA_MAX_TIEMOUT_LIMIT	500
#    define DOWN_ARENA_COUNT_READ_SOCKET	100

extern void downArena_start();
extern void downArena_draw();
extern void downArena_event();
extern void downArena_stop();
extern void downArena_init();
extern void downArena_quit();

#endif
