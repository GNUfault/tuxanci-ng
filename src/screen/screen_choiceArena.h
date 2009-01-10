
#ifndef SCREEN_CHOICE_ARENA

#    define SCREEN_CHOICE_ARENA

#    include "main.h"
#    include "arenaFile.h"

#    define SCREENSHOT_ARENA_WIDTH		160
#    define SCREENSHOT_ARENA_HEIGHT		107

extern void drawScreenChoiceArena();
extern void eventScreenChoiceArena();
extern arenaFile_t *getChoiceArena();
extern void setChoiceArena(arenaFile_t * arenaFile);
extern void initScreenChoiceArena();
extern void quitScreenChoiceArena();

#endif
