#ifndef SCREEN_GAME_TYPE_H
#define SCREEN_GAME_TYPE_H

#include "main.h"

extern void game_type_draw();
extern void game_type_event();
extern void game_type_init();
extern int setSettingGameType(int status);
extern int public_server_get_settingGameType();
extern char *game_type_load_session();
extern int game_type_set_ip(char *address);
extern char *public_server_get_settingIP();
extern int game_type_set_port(int port);
extern int public_server_get_settingPort();
extern int public_server_get_settingProto();
extern void game_type_quit();

#endif /* SCREEN_GAME_TYPE_H */
