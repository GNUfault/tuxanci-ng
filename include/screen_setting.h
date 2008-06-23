
#ifndef SCREEN_SETTING

#define SCREEN_SETTING

extern void initScreenSetting();
extern void drawScreenSetting();
extern void eventScreenSetting();
extern void getSettingNameRight(char *s);
extern void getSettingNameLeft(char *s);
extern void getSettingCountRound(int *n);
extern char* getSettingAI();
extern bool_t isSettingAnyItem();
extern bool_t isSettingItem(int item);
extern void quitScreenSetting();

#endif

