#ifndef MEM_DEBUG_H
#define MEM_DEBUG_H
#include "util.h"
#include "vec.h"

/*
    Memory debugger
    Works for Windows Threads, may not work for mutex locks.
    Untested for POSIX Threads.

    Not meant to free thread/handles only the memory allocated on them.
*/

/* set to 1 to enable memory replacement macros */
#define DEBUG_MEM 1

/* set to 1 to enable printing when there are no errors */
#define PRINT_ALL 0

/*
   Use these twp macros to run the memory debugger
   You can ignore the rest of the file unless you are interest in implementation
*/

/*
    MUST BE PLACED AT ABSOLUTE END OF MAIN FUNCTION!
    (Immediatley before returning)
    End mem debugging and print all unfreed pointers.
    Will also free all unfreed memory.
*/
#define MEM_DEBUG_END(stream) (find_leaks(stream))

/*
    Print info of all pointers currently allocated
    File Name must be less than 100 chars long
*/
#define MEM_DEBUG_INSPECT(stream) (debug_print_table(stream, __LINE__, __FILE__))

/* Implementation below */

/* if a ptr is realloced we set the name to the ptrs name */
typedef struct
{
    bool known;
    char *name;
} Name;

/* every time we allocate memory we save a Debug_Data to a Vec */
typedef struct
{
    void *data;
    char *file;
    size_t size_bytes;
    uint32 line;
    uint32 realloc_count;
    Name alias;
} Debug_Data;

Comparison debug_data_cmp(const Debug_Data *a, const Debug_Data *b)
{
    if ((a->data == b->data) ||
        ((a->line == b->line) && (a->file == b->file)))
        return EQUAL;
    if (a->line < b->line)
        return LESS;
    return GREATER;
}

/* define Vec_Mem */
DEFINE_VEC(Debug_Data, Vec_Mem, debug_data_cmp);

/* define static variable (initialized when including header) */

static Vec_Mem debug_vec =
    {
        .data = 0,
        .size = 0,
        .capacity = 0,
};

#define PICK_COLOR (i % 2 == 0 ? CYN : MAG)

void print_char_x_times(FILE *stream, char ch, int num)
{
    for (int i = 0; i < num; i++)
    {
        fputc(ch, stream);
    }
}

#define BAR_LENGTH 159
void debug_print_table(FILE *stream, uint32_t line, char *file)
{
    size_t i;
    size_t size = vec_size_Vec_Mem(&debug_vec);
    assert(size != SIZE_MAX);

    // Print table header
    fprintf(stream, MAG "Beginning Memory Inspection: " YEL "line %d" WHT " in" YEL " file %s \n", line, file);
    fprintf(stream, "%s| %16s | %15s | %13s | %15s | %s |\n", MAG, "Pointer Address", "Alloc Line", "Realloc Count", "Data Size (Bytes)", "File");

    print_char_x_times(stream, '-', BAR_LENGTH);
    fprintf(stream, "%s\n", MAG);

    for (i = 0; i < size; i++)
    {
        char *color = PICK_COLOR;
        Debug_Data *d = unpack_Debug_Data(vec_at_Vec_Mem(&debug_vec, i));
        fprintf(stream, "%s| %p | %15d | %13d | %15lu | %s |\n" RESET,
                color, d->data, d->line, d->realloc_count, d->size_bytes, d->file);
    }

    fprintf(stream, "%s", PICK_COLOR);
    print_char_x_times(stream, '-', BAR_LENGTH);
    fprintf(stream, "\nEnd of Memory Inspection: " YEL "line %d" WHT " in" YEL " file %s \n", line, file);
}

void debug_inspect_memory(FILE *stream)
{
    size_t i;
    size_t size = vec_size_Vec_Mem(&debug_vec);
    assert(size != SIZE_MAX);
    fprintf(stream, "%sBeginning Memory Inspection.\n", MAG);
    for (i = 0; i < size; i++)
    {
        char *color = PICK_COLOR;
        Debug_Data *d = unpack_Debug_Data(vec_at_Vec_Mem(&debug_vec, i));
        fprintf(stream, "%sPointer %p allocated in file: %s, on line: %d, has been realloced %d times.\n" RESET,
                color, d->data, d->file, d->line, d->realloc_count);
    }
    fprintf(stream, "%sEnd of Memory Inspection.\n" RESET, PICK_COLOR);
}

void init_leak_finder(void)
{
    fprintf(stderr, GRN "Beginning Memory Debug.\n" RESET);
    debug_vec = create_Vec_Mem();
}

/* malloc acts as a constructor for Debug_Data */
void *debug_malloc(size_t size, uint32 line, char *file, char *resize_statement)
{
    void *ptr = malloc(size);
#if PRINT_ALL
    fprintf(stderr,
            BLU "debug_malloc called on line %d, in file %s. Statement used to resize was (%s) is equal to %d bytes.\n" RESET,
            line, file, resize_statement, size);
#endif
    Name n = {.known = false, .name = "Lengthy_Placeholder"};
    Debug_Data d = {.data = ptr, .size_bytes = size, .file = file, .line = line, .realloc_count = 0, .alias = n};
    vec_push_back_Vec_Mem(&debug_vec, d);
    return ptr;
}

/*
    The ptr we get in realloc should be the same we gave in malloc
    but we will give a new ptr back and just update the debug_data and not
    the line or file so we know where it came from
    Increment the realloc count
*/
void *debug_realloc(void *ptr, size_t size, uint32 line, char *file, char *resize_statement, char *name)
{
    void *new_ptr = realloc(ptr, size);
    Debug_Data in = {ptr};
    Debug_Data *d = unpack_Debug_Data(vec_find_Vec_Mem(&debug_vec, in));
#if PRINT_ALL
    fprintf(stderr,
            YEL "debug_realloc called on line %d, in file %s. Statement used to resize was (%s) is equal to %d bytes. Pointer named (%s) has been resized %d times.\n" RESET,
            line, file, resize_statement, size, name, d->realloc_count + 1);
#endif
    d->alias.name = name;
    d->alias.known = true;
    d->size_bytes = size;
    d->realloc_count++;
    d->data = new_ptr;
    return new_ptr;
}

/* should act as constructor like malloc */
void *debug_calloc(size_t num, size_t size, uint32 line, char *file, char *resize_statement)
{
    void *ptr = calloc(num, size);
#if PRINT_ALL
    fprintf(stderr,
            YEL "debug_calloc called on line %d, in file %s. Statement used to resize was (%s) is equal to %d bytes.\n" RESET,
            line, file, resize_statement, size);
#endif
    Name n = {.known = false, .name = "Lengthy_Placeholder"};
    Debug_Data d = {.data = ptr, .size_bytes = size, .file = __FILE__, .line = __LINE__, .realloc_count = 0, .alias = n};
    vec_push_back_Vec_Mem(&debug_vec, d);
    return ptr;
}

/* should update to check if you incremented the ptr and tried to free that ptr instead of the original head */
/* freak out if we try to free data not in the Vec */
void debug_free(void *ptr, uint32 line, char *file, char *var_name)
{
    Debug_Data find = {ptr};
    Return_Debug_Data ret = vec_find_Vec_Mem(&debug_vec, find);
    if (ret.err == VEC_CANT_FIND_ERROR)
    {
        fprintf(stderr,
                RED "debug_free attempting to free non-dynamic/stack memory on (line %d, file %s). Variable named (%s)\n" RESET,
                line, file, var_name);
    }
    if (ret.err != VEC_OK)
        return;
    Debug_Data *found = unpack_Debug_Data(ret);
#if PRINT_ALL
    fprintf(stderr,
            GRN "debug_free freeing variable on (line %d, file %s) from (line %d, file %s). Pointer %p was resized %d times.\n" RESET,
            line, file, found->line, found->file, var_name, found->realloc_count);
#endif
    ASSERT_ON_ERROR(vec_remove_Vec_Mem(&debug_vec, ret.index), "debug_free error freeing debug_vec, likely freed unallocated memory.");
    free(ptr);
}

/* print all unfreed pointers */
void find_leaks(FILE *stream)
{
    int i;
    for (i = 0; i < vec_size_Vec_Mem(&debug_vec); i++)
    {
        char *n = "Unknown_Ptr_Alias";
        Debug_Data *d = unpack_Debug_Data(vec_at_Vec_Mem(&debug_vec, i));
        if (d->alias.known)
            n = d->alias.name;
        fprintf(stream, RED "Pointer '%s' at address %p not freed." WHT " Allocated on " YEL "line %d" WHT " in" YEL " file %s.\n" RESET,
                n, d->data, d->line, d->file);
        free(d->data);
    }
    vec_free_Vec_Mem(&debug_vec);
    if (i == 0)
        fprintf(stream, GRN "No Leaks Found :)\n" RESET);
    else
        fprintf(stream, RED "Found & Patched %d leaks.\n" RESET, i);
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