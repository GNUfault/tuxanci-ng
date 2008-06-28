
#ifndef TERM_H

#define TERM_H

#include "base/main.h"

#define TERM_MAX_LINES		10
#define TERM_DEL_TEXT_TIMEOUT	2000

extern void initTerm();
extern void drawTerm();
extern void eventTerm();
extern void appendTextInTerm(char *s);
extern void quitTerm();

#endif
