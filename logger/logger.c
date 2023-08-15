#include "logger.h"
#include <stdarg.h>
#include <stdlib.h>
#include <time.h>

/* ctime seems to have a \n at the end */
char *datetime(void) {
  time_t t;
  time(&t);
  return ctime(&t);
}

LogFile _LogOpen(const char *filename, const char *cfile, const int line) {
  LogFile logger = {.isOpen = true};
  logger.file = fopen(filename, "a");
  fprintf(logger.file, "\n+-----+\n Log Opened - %s - File: %s - Line: %d\n", datetime(),
          cfile, line);
  return logger;
}

LogFile _LogClose(LogFile logger, const char *cfile, const int line) {
  fprintf(logger.file, "\nLog Closed - %s - File: %s - Line: %d\n+-----+\n", datetime(),
          cfile, line);
  fclose(logger.file);
  logger.isOpen = false;
  return logger;
}

void _Log(LogFile logger, const char *msg, const char *cfile, const int line) {
  if (logger.isOpen) {
    fprintf(logger.file, "\n%s - File: %s - Line: %d | %s\n", datetime(),
            cfile, line, msg);
  } else {
    fprintf(stderr, "\n%s - Could not log message \"%s\", log file not open\n",
            datetime(), msg);
  }
}

void _Logf(LogFile logger, const char *msg, const char *cfile, const int line, ...) {
  if (logger.isOpen) {
    fprintf(logger.file, "\n%s - File: %s - Line: %d | ", datetime(),
            cfile, line);
    va_list v;
    va_start(v,strlen(msg));
    vfprintf(logger.file, msg,v);
    va_end(v);
  } else {
    fprintf(stderr, "%s - Could not log message \"%s\", log file not open",
            datetime(), msg);
  }
}
