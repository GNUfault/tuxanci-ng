
#ifndef SCREEN_CHOICE_ARENA

#    define SCREEN_CHOICE_ARENA

#    include "main.h"
#    include "arenaFile.h"

#    define SCREENSHOT_ARENA_WIDTH		160
#    define SCREENSHOT_ARENA_HEIGHT		107

extern void choiceArena_draw();
extern void choiceArena_event();
extern arenaFile_t *choiceArena_get();
extern void choiceArena_set(arenaFile_t * arenaFile);
extern void choiceArena_init();
extern void choiceArena_quit();

#endif
