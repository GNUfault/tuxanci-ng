
#ifndef LOG_H

#    define LOG_H

#    define LOG_INF		1
#    define LOG_DBG		2
#    define LOG_WRN		3
#    define LOG_ERR		4

extern int log_init(char *name);
extern void log_add(int type, char *msg);
extern void log_quit();

#endif
