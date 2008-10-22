
#ifndef SCREEN_ANALYZE_H

#define SCREEN_ANALYZE_H

#include "main.h"

typedef struct analyze_struct
{
    char *name;
    int score;
} analyze_t;

extern void startScreenAnalyze();
extern void drawScreenAnalyze();
extern void eventScreenAnalyze();
extern void stopScreenAnalyze();
extern void restartAnalyze();
extern void addAnalyze(char *name, int score);
extern void setMsgToAnalyze(char *msg);
extern void endAnalyze();
extern void initScreenAnalyze();
extern void quitScreenAnalyze();

#endif
