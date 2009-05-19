#ifndef SCREEN_SETTING_H
#define SCREEN_SETTING_H

#include "main.h"

#define NAME_AI			"TuxBot"
#define NAME_PLAYER_RIGHT	"Name1"
#define NAME_PLAYER_LEFT	"Name2"

extern void setting_init();
extern void setting_draw();
extern void setting_event();
extern void public_server_get_setting_name_right(char *s);
extern void public_server_get_settingNameLeft(char *s);
extern void public_server_get_settingCountRound(int *n);
extern bool_t setting_is_ai();
extern bool_t setting_is_any_item();
extern bool_t setting_is_item(int item);
extern void setting_quit();

#endif /* SCREEN_SETTING_H */
