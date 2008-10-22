
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <assert.h>

#include "log.h"
#include "main.h"

static FILE *logFile;

int
initLog(char *name)
{
    logFile = fopen(name, "a");

    if (logFile == NULL) {
        fprintf(stderr, _("Opening logfile \"%s\" failed!\n"), name);
        return -1;
    }
#ifdef DEBUG
    printf(_("I use logfile: \"%s\")\n"), name);
#endif
    addToLog(LOG_INF, "open log file");

    return 0;
}

void
addToLog(int type, char *msg)
{
    char str[STR_LOG_SIZE];
    struct tm *tm_struct;
    time_t currentTime;
    char *str_type;

    currentTime = time(NULL);
    tm_struct = localtime(&currentTime);

    switch (type) {
    case LOG_INF:
        str_type = "INF";
        break;
    case LOG_DBG:
        str_type = "DBG";
        break;
    case LOG_WRN:
        str_type = "WRN";
        break;
    case LOG_ERR:
        str_type = "ERR";
        break;
    default:
        assert(!_("Really bad log value!"));
        break;
    }

    sprintf(str, "(%02d-%02d-%02d %02d:%02d:%02d) %s %s\n",
            1900 + tm_struct->tm_year, tm_struct->tm_mon, tm_struct->tm_mday,
            tm_struct->tm_hour, tm_struct->tm_min, tm_struct->tm_sec,
            str_type, msg);

    fprintf(logFile, str);

    fflush(logFile);
}

void
quitLog()
{
    addToLog(LOG_INF, "close log file");
    fclose(logFile);
}
