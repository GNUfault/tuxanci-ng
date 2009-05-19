#ifndef HIGH_SCORE_H
#define HIGH_SCORE_H

#define HIGHSCORE_MAX_PLAYERS	100

extern void high_score_init(char *file);
extern int table_add(char *name, int score);
extern char *high_score_get_table(int index);
extern void high_score_quit();

#endif /* HIGH_SCORE_H */
