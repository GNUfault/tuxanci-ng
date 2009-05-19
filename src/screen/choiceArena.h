#ifndef SCREEN_CHOICE_ARENA_H
#define SCREEN_CHOICE_ARENA_H

#include "main.h"
#include "arenaFile.h"

#define SCREENSHOT_ARENA_WIDTH		160
#define SCREENSHOT_ARENA_HEIGHT		107

extern void choice_arena_draw();
extern void choice_arena_event();
extern arenaFile_t *choice_arena_get();
extern void choice_arena_set(arenaFile_t *arenaFile);
extern void choice_arena_init();
extern void choice_arena_quit();

#endif /* SCREEN_CHOICE_ARENA_H */
