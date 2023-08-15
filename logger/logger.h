#ifndef _LOG_H
#define _LOG_H

#include <stdbool.h>
#include <stdio.h>

typedef struct LogFile LogFile;

struct LogFile
{
  FILE *file;
  unsigned int entries;
};

#define LogOpen(filename) _LogOpen(filename, __FILE__, __LINE__)
#define LogClose(logger) _LogClose(logger, __FILE__, __LINE__)
#define Log(logger, msg) _Log(logger, msg, __FILE__, __LINE__)
#define Logf(logger, format, ...) _Logf(logger, format, __FILE__, __LINE__, __VA_ARGS__)

LogFile *_LogOpen(const char *filename, const char *cfile, const int line);
void _LogClose(LogFile *logger, const char *cfile, const int line);
void _Log(LogFile *logger, const char *msg, const char *cfile, const int line);
void _Logf(LogFile *logger, const char *msg, const char *cfile, const int line, ...);

#endif /* _LOG_H */