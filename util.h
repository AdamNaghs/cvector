#ifndef UTIL_H
#define UTIL_H

#include <stdint.h>
#include <assert.h>

#define RED "\033[31m"
#define GRN "\033[32m"
#define YEL "\033[33m"
#define BLU "\033[34m"
#define MAG "\033[35m"
#define CYN "\033[36m"
#define WHT "\033[37m"
#define RESET "\033[0m"

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;
typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;


typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;
typedef float f32;
typedef double f64;
typedef long double f128;




#ifndef ASSERT_PRINT
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
#endif
#ifndef FPRINTF_ASSERT
#define FPRINTF_ASSERT(stream, cond, fstring, ...)            \
  do                                                          \
  {                                                           \
    if (!(cond))                                              \
    {                                                         \
      fprintf(stream, RED fstring RESET, #cond, __VA_ARGS__); \
      assert(cond);                                           \
    }                                                         \
  } while (0)
#endif


/* fast square root*/
float inv_sqrt(float number)
{
    int i;
    float x, y;
    x = number * 0.5;
    y = number;
    i = *(int *)&y;
    i = 0x5f3759df - (i >> 1);
    y = *(float *)&i;
    y = y * (1.5 - (x * y * y));
    y = y * (1.5 - (x * y * y));
    return number * y;
}
float f_sqrt(float number)
{
    return 1 / inv_sqrt(number);
}
/* random number generator */
uint32 u_randi(uint32 index)
{
    index = (index << 13) ^ index;
    return ((index * (index * index * 15731 + 789221) + 1376312589) & 0x7fffffff); /* & 0x7fffffff ensures this is less than uint32 max */
}

#endif /* UTIL_H */