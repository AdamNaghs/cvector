#include "logger.h"
#include <stdarg.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>

#define LOG_HEADER "\n+----------+\n"

#define NULL_LOG_ERR(logger_ptr, cfile, line)                                                  \
  do                                                                                           \
  {                                                                                            \
    if (!logger_ptr)                                                                           \
    {                                                                                          \
      fprintf(stderr, "Attempting to access NULL LogFile on line %d in file %s", line, cfile); \
      assert(logger_ptr != NULL);                                                              \
    }                                                                                          \
  } while (0)

/* ctime seems to have a \n at the end */
char *datetime(void)
{
  time_t t;
  time(&t);
  return ctime(&t);
}

LogFile *_LogOpen(const char *filename, const char *cfile, const int line)
{
  LogFile *logger = (LogFile *)malloc(sizeof(LogFile));
  logger->entries = 0;
  logger->file = fopen(filename, "a");
  fprintf(logger->file, LOG_HEADER "Log Opened - File: %s - Line: %d - %s",
          cfile, line, datetime());
  return logger;
}

void _LogClose(LogFile *logger, const char *cfile, const int line)
{
  NULL_LOG_ERR(logger, cfile, line);
  fprintf(logger->file, "\nLog Closed - File: %s - Line: %d - %s" "+----------+\n",
          cfile, line, datetime());
  fclose(logger->file);
  free(logger);
}

void _Log(LogFile *logger, const char *msg, const char *cfile, const int line)
{
  NULL_LOG_ERR(logger, cfile, line);
  logger->entries++;
  fprintf(logger->file, "\n%d | %s  | File: %s - Line: %d \n  -> %s\n", logger->entries, datetime(),
          cfile, line, msg);
}

void _Logf(LogFile *logger, const char *msg, const char *cfile, const int line, ...)
{
  NULL_LOG_ERR(logger, cfile, line);
  logger->entries++;
  fprintf(logger->file, "\n%d | %s  | File: %s - Line: %d \n  -> ", logger->entries, datetime(),
          cfile, line);
  va_list v;
  va_start(v, line); /* line is last named arg */
  vfprintf(logger->file, msg, v);
  fputc('\n', logger->file);
  va_end(v);
}
