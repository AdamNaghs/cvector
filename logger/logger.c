#include "logger.h"
#include <stdarg.h>
#include <stdlib.h>
#include <time.h>

/* ctime seems to have a \n at the end */
char *datetime(void)
{
  time_t t;
  time(&t);
  return ctime(&t);
}

LogFile *_LogOpen(const char *filename, const char *cfile, const int line)
{
  LogFile *logger = malloc(sizeof(LogFile));
  logger->entries = 0;
  logger->file = fopen(filename, "a");
  logger->isOpen = true;
  fprintf(logger->file, "\n+-----+\n Log Opened - %s - File: %s - Line: %d\n", datetime(),
          cfile, line);
  return logger;
}

void _LogClose(LogFile *logger, const char *cfile, const int line)
{
  fprintf(logger->file, "\nLog Closed - %s - File: %s - Line: %d\n+-----+\n", datetime(),
          cfile, line);
  fclose(logger->file);
  free(logger);
}

void _Log(LogFile *logger, const char *msg, const char *cfile, const int line)
{
  if (logger->isOpen)
  {
    logger->entries++;
    fprintf(logger->file, "\n%d | %s  | File: %s - Line: %d \n  -> %s\n", logger->entries, datetime(),
            cfile, line, msg);
  }
  else
  {
    fprintf(stderr, "\n%s - Could not log message \"%s\", log file not open\n",
            datetime(), msg);
  }
}

void _Logf(LogFile *logger, const char *msg, const char *cfile, const int line, ...)
{
  if (logger->isOpen)
  {
    logger->entries++;
    fprintf(logger->file, "\n%d | %s  | File: %s - Line: %d \n  -> ", logger->entries, datetime(),
            cfile, line);
    va_list v;
    va_start(v, line); /* line is last named arg */
    vfprintf(logger->file, msg, v);
    fputc('\n', logger->file);
    va_end(v);
  }
  else
  {
    fprintf(stderr, "%s - Could not log message \"%s\", log file not open",
            datetime(), msg);
  }
}
