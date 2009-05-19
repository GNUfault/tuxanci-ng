#ifndef SCREEN_TABLE_H
#define SCREEN_TABLE_H

#define SCREEN_TABLE_MAX_PLAYERS		10
#define SCREEN_TABLE_FILE_HIGHSCORE_NAME	"highscore"

extern void table_start();
extern void table_draw();
extern void table_event();
extern void table_stop();
extern int table_add(char *name, int score);
extern void table_init();
extern void table_quit();

#endif /* SCREEN_TABLE_H */
