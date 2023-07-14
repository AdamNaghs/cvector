#ifndef ASSERT_INTERNAL_H
#define ASSERT_INTERNAL_H

#include <assert.h>
#include <stdio.h>

#define RED "\x1B[31m"
#define GRN "\x1B[32m"
#define YEL "\x1B[33m"
#define BLU "\x1B[34m"
#define MAG "\x1B[35m"
#define CYN "\x1B[36m"
#define WHT "\x1B[37m"
#define RESET "\x1B[0m"

#define ASSERT_PRINT(cond)                                           \
  do                                                                 \
  {                                                                  \
    if (!(cond))                                                     \
    {                                                                \
      fprintf(stderr, RED "Assertion failed:  '%s'\n" RESET, #cond); \
      assert(cond);                                                  \
    }                                                                \
    else                                                             \
    {                                                                \
      fprintf(stderr, GRN "Assertion passed:  '%s'\n" RESET, #cond); \
      assert(cond);                                                  \
    }                                                                \
  } while (0)

#define PRINTF_ASSERT(cond,fstring,...)                                           \
  do                                                                 \
  {                                                                  \
    if (!(cond))                                                     \
    {                                                                \
      fprintf(stderr, RED "Assertion failed:  '%s'\n" RESET, #cond); \
      assert(cond);                                                  \
    }                                                                \
    else                                                             \
    {                                                                \
      fprintf(stderr, GRN "Assertion passed:  '%s' " fstring RESET, #cond,__VA_ARGS__); \
      assert(cond);                                                  \
    }                                                                \
  } while (0)


#endif /* ASSERT_INTERNAL_H */