
#ifndef SCREEN_GAME_TYPE

#    define SCREEN_GAME_TYPE

#    include "main.h"

extern void drawScreenGameType();
extern void eventScreenGameType();
extern void initScreenGameType();
extern int setSettingGameType(int status);
extern int getSettingGameType();
extern char *getGemeTypeLoadSession();
extern int setSettingIP(char *address);
extern char *getSettingIP();
extern int setSettingPort(int port);
extern int getSettingPort();
extern int getSettingProto();
extern void quitScreenGameType();

#endif
