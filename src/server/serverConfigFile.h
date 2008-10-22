

#ifndef SERVER_CONFIG_FILE_H

#define SERVER_CONFIG_FILE_H

extern void initServerConfigFile();
extern char *getServerConfigFileValue(char *env, char *s);
extern void quitServerConfigFile();

#endif
