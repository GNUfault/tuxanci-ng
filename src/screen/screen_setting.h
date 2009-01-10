
#ifndef SCREEN_SETTING

#    define SCREEN_SETTING

#    include "main.h"

#    define NAME_AI			"TuxBot"
#    define NAME_PLAYER_RIGHT	"Name1"
#    define NAME_PLAYER_LEFT	"Name2"

extern void initScreenSetting();
extern void drawScreenSetting();
extern void eventScreenSetting();
extern void getSettingNameRight(char *s);
extern void getSettingNameLeft(char *s);
extern void getSettingCountRound(int *n);
extern bool_t isSettingAI();
extern bool_t isSettingAnyItem();
extern bool_t isSettingItem(int item);
extern void quitScreenSetting();
#endif
