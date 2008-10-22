
#ifndef HEIGHT_SCORE_H

#define HEIGHT_SCORE_H

#define HEIGHTSCORE_MAX_PLAYERS	100

extern void initHeightScore(char *file);
extern int addPlayerInHighScore(char *name, int score);
extern char *getTableItem(int index);
extern void quitHeightScore();

#endif
