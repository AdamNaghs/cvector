#ifndef MEM_DEBUG_H
#define MEM_DEBUG_H
#include "colored_assert.h"
#include "util.h"
#include "vec.h"

/* simple single threaded memory debugger */

/* whatever file this is included in need to
    have a Vec_Mem debug_vec;
    and init that vec by calling init_leak_finder();
*/

/* set to 1 to enable */
#define DEBUG_MEM 1

#define PRINT_ALL 1

typedef struct
{
    void *data;
    char *file;
    uint32 line;
    uint32 realloc_count;
} Debug_Data;

/* define Vec_Mem*/
DEFINE_VEC(Debug_Data, Vec_Mem);

extern Vec_Mem debug_vec;

Comparison debug_data_cmp(const Debug_Data *a, const Debug_Data *b)
{
    if ((a->data == b->data) ||
        ((a->line == b->line) && (a->file == b->file)))
        return EQUAL;
    if (a->line < b->line)
        return LESS;
    return GREATER;
}

void init_leak_finder(void)
{
    debug_vec = create_Vec_Mem(debug_data_cmp);
}

/* To keep track of the memory we are createing this struct
    to hold a void ptr and __LINE__ and __FILE__
    since the data is the first thing in the struct
    we can cast it directly to it (hopefully implicitley)

    Each time any function returns a void* it will be wrapped in a Debug_Data struct
    we need to add this data to an array and once it is freed remove it, then at the end
    report all memory left unfreed.
*/

/* malloc should act as a constructor for Debug_Data */

void *debug_malloc(size_t size, uint32 line, char *file, char *resize_statement)
{
    void *ptr = malloc(size);
    if (PRINT_ALL)
        fprintf(stderr,
                GRN"debug_malloc called on line %d, in file %s. Statement used to resize was (%s) is equal to %d.\n"RESET,
                line, file, resize_statement,size);
    Debug_Data d = {ptr, file, line, 0};
    debug_vec.push_back(&debug_vec, d);
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
    Debug_Data in = {ptr};
    Debug_Data *d = unpack_Debug_Data(debug_vec.find(&debug_vec, in));
    if (PRINT_ALL)
        fprintf(stderr,
                GRN"debug_realloc called  on line %d, in file %s. Statement used to resize was (%s) is equal to %d. Pointer has been resized %d times.\n"RESET,
                 line, file, resize_statement,size, d->realloc_count+1);

    d->data = new_ptr;
    d->realloc_count++;
    return new_ptr;
}

/* should act as constructor like malloc */

void *debug_calloc(size_t num, size_t size, uint32 line, char *file, char *resize_statement)
{
    void *ptr = calloc(num, size);
    if (PRINT_ALL)
        fprintf(stderr,
                GRN"debug_calloc called on line %d, in file %s. Statement used to resize was (%s) is equal to %d.\n"RESET,
                 line, file, resize_statement,size);
    Debug_Data d = {ptr, __FILE__, __LINE__, 0};
    debug_vec.push_back(&debug_vec, d);
    return ptr;
}

/* freak out if we didnt get Debug_data */

void debug_free(void *ptr, uint32 line, char *file, char *var_name)
{
    Debug_Data find = {ptr};
    Return_Debug_Data ret = debug_vec.find(&debug_vec, find);
    if (ret.err == VEC_CANT_FIND_ERROR) {
        fprintf(stderr,
            RED"debug_free attempting to free non-dynamic/stack memory on (line %d, file %s). Variable named (%s)\n"RESET,
            line, file, var_name);
    }
    Debug_Data *found = unpack_Debug_Data(ret);
    if (PRINT_ALL)
        fprintf(stderr,
                GRN"debug_free freeing variable on (line %d, file %s) from (line %d, file %s). Pointer name was %p.\n"RESET,
                line, file, found->line, found->file, var_name);
    //ASSERT_ON_ERROR(debug_vec.remove(&debug_vec, ret.index), "error freeing debug_vec, likely freed unallocated memory.");
    free(ptr);
}

/* write func which should be ran at the end of program to report all unfreed data
    and print to stderr the original line and file it was allocated on
*/

void find_leaks(void)
{
    int i;
    for (i = 0; i < debug_vec.size; i++)
    {
        Debug_Data *d = unpack_Debug_Data(debug_vec.at(&debug_vec, i));
        fprintf(stderr, RED "Pointer at address %p not freed. Initialized on line %d in file %s.\n" RESET,
                d->data, d->line, d->file);
    }
    debug_vec.free(&debug_vec);
}

#if DEBUG_MEM == 1
#define malloc(size) (debug_malloc(size, __LINE__, __FILE__, #size))
#define realloc(ptr, size) (debug_realloc(ptr, size, __LINE__, __FILE__, #size))
#define calloc(num, size) (debug_calloc(num, size, __LINE__, __FILE__, #size))
#define free(ptr) (debug_free(ptr, __LINE__, __FILE__, #ptr))
#endif /* DEBUG_MEM */

#endif /* MEM_DEBUG_H */