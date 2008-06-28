
#ifndef CONFIGFILE_H

#define CONFIGFILE_H

#include "base/main.h"
#include "base/textFile.h"

extern int isYesOrNO(char *s);
extern char* getYesOrNo(int n);

extern int getValue(char *line, char *env, char *val, int len);
extern char* setValue(char *line, char *env, char *val);

extern int getValueInConfigFile(textFile_t *textFile, char *env, char *val, int len);
extern int setValueInConfigFile(textFile_t *textFile, char *env, char *val);

extern void loadValueFromConfigFile(textFile_t *textFile, char *env, char *val, int len, char *butVal);

#endif
