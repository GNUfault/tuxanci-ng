

#ifndef SERVER_CONFIG_FILE_H

#    define SERVER_CONFIG_FILE_H

extern void server_configFile_init();
extern char *server_configFile_get_value(char *env, char *s);
extern void server_configFile_quit();

#endif
