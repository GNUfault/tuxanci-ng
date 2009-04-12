
#ifndef HIGH_SCORE_H

#    define HIGH_SCORE_H

#    define HIGHSCORE_MAX_PLAYERS	100

extern void highScore_init(char *file);
extern int table_add(char *name, int score);
extern char *highScore_get_table(int index);
extern void highScore_quit();

#endif
