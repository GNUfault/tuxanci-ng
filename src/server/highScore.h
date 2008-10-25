
#ifndef HIGH_SCORE_H

#define HIGH_SCORE_H

#define HIGHSCORE_MAX_PLAYERS	100

extern void initHighScore(char *file);
extern int addPlayerInHighScore(char *name, int score);
extern char *getTableItem(int index);
extern void quitHighScore();

#endif
