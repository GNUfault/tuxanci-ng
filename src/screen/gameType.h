
#ifndef SCREEN_GAME_TYPE

#    define SCREEN_GAME_TYPE

#    include "main.h"

extern void gameType_draw();
extern void gameType_event();
extern void gameType_init();
extern int setSettingGameType(int status);
extern int publicServer_get_settingGameType();
extern char *gameType_load_session();
extern int gameType_set_ip(char *address);
extern char *publicServer_get_settingIP();
extern int gameType_set_port(int port);
extern int publicServer_get_settingPort();
extern int publicServer_get_settingProto();
extern void gameType_quit();

#endif
