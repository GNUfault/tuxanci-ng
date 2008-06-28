
#ifndef SCREEN_TABLE_H

#define SCREEN_TABLE_H

#define SCREEN_TABLE_MAX_PLAYERS	10
#define SCREEN_TABLE_FILE_HIGHSCORE_NAME	"highscore"

extern void startScreenTable();
extern void drawScreenTable();
extern void eventScreenTable();
extern void stopScreenTable();
extern int addPlayerInHighScore(char *name, int score);
extern void initScreenTable();
extern void quitScreenTable();

#endif
