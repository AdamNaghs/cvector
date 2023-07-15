#ifndef MEM_DEBUG_H
#define MEM_DEBUG_H
#include "colored_assert.h"
#include "util.h"
#include "vec.h"

/* simple single threaded memory debugger */


/* set to 1 to enable memory replacement macros */
#define DEBUG_MEM 1

/* set to 1 to enable printing when there are no errors */
#define PRINT_ALL 0

/*
   Use these four macros to run the memory debugger
   You can ignore the rest of the file unless you are interest in implementation
*/

/* 
    Place before main function.
    Init global variable. 
*/
#define MEM_DEBUG_INIT Vec_Mem debug_vec

/* 
    MUST BE PLACE AT ABSOLUTE BEGINNING OF MAIN FUNCTION!
    Start saving memory being allocated.
*/
#define MEM_DEBUG_START() init_leak_finder()

/* 
    MUST BE PLACED AT ABSOLUTE END OF MAIN FUNCTION!
    (Immediatley before returning)
    End mem debugging and print all unfreed pointers.
    Will also free all unfreed memory.
*/
#define MEM_DEBUG_END() find_leaks()

/*
    Print info of all pointers currently allocated
*/
#define MEM_DEBUG_INSPECT(stream) debug_inspect_memory(stream)

/* Implementation below */

/* every time we allocate memory we save a Debug_Data to a Vec */
typedef struct
{
    void *data;
    char *file;
    uint32 line;
    uint32 realloc_count;
} Debug_Data;

/* define Vec_Mem*/
DEFINE_VEC(Debug_Data, Vec_Mem);

/* define external variable (initialized with MEM_DEBUG_INIT) */
extern Vec_Mem debug_vec;


#define PICK_COLOR (i % 2 == 0 ? CYN : MAG)

void debug_inspect_memory(FILE* stream)
{
    size_t i;
    size_t size = debug_vec.read_size(&debug_vec);
    assert(size != SIZE_MAX);
    fprintf(stream,"%sBeginning Memory Inspection.\n",MAG);
    for (i = 0; i < size; i++)
    {   
        char* color = PICK_COLOR;
        Debug_Data* d = unpack_Debug_Data(debug_vec.at(&debug_vec,i));
        fprintf(stream,"%sPointer %p allocated in file: %s, on line: %d, has been realloced %d times.\n"RESET,
            color,d->data,d->file,d->line,d->realloc_count);
    }
    fprintf(stream,"%sEnd of Memory Inspection.\n"RESET,PICK_COLOR);
}


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

/* malloc acts as a constructor for Debug_Data */
void *debug_malloc(size_t size, uint32 line, char *file, char *resize_statement)
{
    void *ptr = malloc(size);
    if (PRINT_ALL)
        fprintf(stderr,
                GRN "debug_malloc called on line %d, in file %s. Statement used to resize was (%s) is equal to %d.\n" RESET,
                line, file, resize_statement, size);
    Debug_Data d = {.data = ptr, .file = file, .line = line, .realloc_count = 0};
    debug_vec.push_back(&debug_vec, d);
    return ptr;
}

/*
    the ptr we get in realloc should be the same we gave in malloc
    but we will give a new ptr back and just update the debug_data and not
    the line or file so we know where it came from
    Increment the realloc count
*/
void *debug_realloc(void *ptr, size_t size, uint32 line, char *file, char *resize_statement, char *name)
{
    void *new_ptr = realloc(ptr, size);
    Debug_Data in = {ptr};
    Debug_Data *d = unpack_Debug_Data(debug_vec.find(&debug_vec, in));
    if (PRINT_ALL)
        fprintf(stderr,
                GRN "debug_realloc called  on line %d, in file %s. Statement used to resize was (%s) is equal to %d. Pointer named (%s) has been resized %d times.\n" RESET,
                line, file, resize_statement, size, name, ++(d->realloc_count));

    d->data = new_ptr;
    return new_ptr;
}

/* should act as constructor like malloc */
void *debug_calloc(size_t num, size_t size, uint32 line, char *file, char *resize_statement)
{
    void *ptr = calloc(num, size);
    if (PRINT_ALL)
        fprintf(stderr,
                GRN "debug_calloc called on line %d, in file %s. Statement used to resize was (%s) is equal to %d.\n" RESET,
                line, file, resize_statement, size);
    Debug_Data d = {ptr, __FILE__, __LINE__, 0};
    debug_vec.push_back(&debug_vec, d);
    return ptr;
}

/* freak out if we try to free data not in the Vec */
void debug_free(void *ptr, uint32 line, char *file, char *var_name)
{
    Debug_Data find = {ptr};
    Return_Debug_Data ret = debug_vec.find(&debug_vec, find);
    if (ret.err == VEC_CANT_FIND_ERROR)
    {
        fprintf(stderr,
                RED "debug_free attempting to free non-dynamic/stack memory on (line %d, file %s). Variable named (%s)\n" RESET,
                line, file, var_name);
    }
    Debug_Data *found = unpack_Debug_Data(ret);
    if (PRINT_ALL)
        fprintf(stderr,
                GRN "debug_free freeing variable on (line %d, file %s) from (line %d, file %s). Pointer %p was resized %d times.\n" RESET,
                line, file, found->line, found->file, var_name, found->realloc_count);
    ASSERT_ON_ERROR(debug_vec.remove(&debug_vec, ret.index), "error freeing debug_vec, likely freed unallocated memory.");
    free(ptr);
}

/* print all unfreed pointers */
void find_leaks(void)
{
    int i;
    for (i = 0; i < debug_vec.size; i++)
    {
        Debug_Data *d = unpack_Debug_Data(debug_vec.at(&debug_vec, i));
        fprintf(stderr, RED "Pointer at address %p not freed. Initialized on line %d in file %s.\n" RESET,
                d->data, d->line, d->file);
        free(d->data);
    }
    debug_vec.free(&debug_vec);
}

#if DEBUG_MEM == 1
/* DEBUG MALLOC */
#define malloc(size) (debug_malloc(size, __LINE__, __FILE__, #size))
/* DEBUG REALLOC */
#define realloc(ptr, size) (debug_realloc(ptr, size, __LINE__, __FILE__, #size, #ptr))
/* DEBUG CALLOC */
#define calloc(num, size) (debug_calloc(num, size, __LINE__, __FILE__, #size))
/* DEBUG FREE */
#define free(ptr) (debug_free(ptr, __LINE__, __FILE__, #ptr))
#endif /* DEBUG_MEM */

#endif /* MEM_DEBUG_H */