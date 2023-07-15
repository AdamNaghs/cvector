#ifndef MEM_DEBUG_H
#define MEM_DEBUG_H
#include "colored_assert.h"
#include "util.h"

/* set to 1 to enable */
#define DEBUG_MEM 1

/* To keep track of the memory we are createing this struct
    to hold a void ptr and __LINE__ and __FILE__
    since the data is the first thing in the struct
    we can cast it directly to it (hopefully implicitley)

    Each time any function returns a void* it will be wrapped in a Debug_Data struct
    we need to add this data to an array and once it is freed remove it, then at the end
    report all memory left unfreed.
*/

typedef struct
{
    void *data;
    char *file;
    uint32 line;
    uint32 realloc_count;
} Debug_Data;

/* malloc should act as a constructor for Debug_Data */

void *debug_malloc(size_t size, uint32 line, char *file, char *resize_statement)
{
    void *ptr = malloc(size);
    FPRINTF_ASSERT(stderr, ptr != NULL,
                   "debug_malloc malloc returned NULL PTR when passed size %d, on line %d, in file %s. Statement used to resize was %s",
                   size, line, file, resize_statement);
    return ptr;
}

/*
    the ptr we get in realloc should be the same we gave in malloc
    but we will give a new ptr back and just update the debug_data and not
    the line so we know where it came from
    Increment the realloc count
*/
void *debug_realloc(void *ptr, size_t size, uint32 line, char *file, char *resize_statement)
{
    void *new_ptr = realloc(ptr, size);
    FPRINTF_ASSERT(stderr, new_ptr != NULL,
                   "debug_realloc realloc returned NULL PTR when passed size %d, on line %d, in file %s. Statement used to resize was %s",
                   size, line, file, resize_statement);
    return new_ptr;
}

/* should as constructor like malloc */

void *debug_calloc(size_t num, size_t size, uint32 line, char *file, char *resize_statement)
{
    void *ptr = calloc(num, size);
    FPRINTF_ASSERT(stderr, ptr != NULL,
                   "debug_calloc calloc returned NULL PTR when passed size %d, on line %d, in file %s. Statement used to resize was %s",
                   size, line, file, resize_statement);
    return ptr;
}

/* freak out if we didnt get Debug_data */

void *debug_free(void *ptr, uint32 line, char *file, char *var_name)
{
    FPRINTF_ASSERT(stderr, ptr != NULL,
                   "debug_free free attempting to free NULL PTR, on line %d, in file %s. Pointer name was %s,\n",
                   line, file, var_name);
    free(ptr);
}

/* write func which should be ran at the end of program to report all unfreed data 
    and print to stderr the original line and file it was allocated on
*/

#if DEBUG_MEM == 1
#define malloc(size) (debug_malloc(size, __LINE__, __FILE__, #size))
#define realloc(ptr, size) (debug_realloc(ptr, size, __LINE__, __FILE__, #size))
#define calloc(num, size) (debug_calloc(num, size, __LINE__, __FILE__, #size))
#define free(ptr) (debug_free(ptr, __LINE__, __FILE__, #ptr))
#endif /* DEBUG_MEM */

#endif /* MEM_DEBUG_H */