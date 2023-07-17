#ifndef UTIL_H
#define UTIL_H

#include <stdint.h>

#define RED "\x1B[31m"
#define GRN "\x1B[32m"
#define YEL "\x1B[33m"
#define BLU "\x1B[34m"
#define MAG "\x1B[35m"
#define CYN "\x1B[36m"
#define WHT "\x1B[37m"
#define RESET "\x1B[0m"

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;
typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

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