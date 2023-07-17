#include <windows.h>
#include <stdio.h>
#include "../mem_debug.h"

#define NUM_THREADS 5
#define NUM_ALLOCATIONS 10
#define ALLOCATION_SIZE 100

// Thread function
DWORD WINAPI allocateMemory(LPVOID param)
{
    for (int i = 0; i < NUM_ALLOCATIONS; i++)
    {
        // Allocate memory but do not free it
        char *memory = (char *)malloc(ALLOCATION_SIZE);
        if (memory == NULL)
        {
            printf("Failed to allocate memory\n");
            return 1;
        }
        // Uncomment the following line to fix the memory leak
        // free(memory);
    }
    return 0;
}

Comparison MyCompareObjectHandles(HANDLE const *hFirst, HANDLE const *hSecond)
{
    return (hFirst == hSecond) ? EQUAL : LESS;
}

DEFINE_VEC(HANDLE, Threads, MyCompareObjectHandles);

int main()
{
    Threads vec = create_Threads();
    // Create the threads
    for (int i = 0; i < NUM_THREADS; i++)
    {
        vec.push_back(&vec, CreateThread(NULL, 0, allocateMemory, NULL, 0, NULL));
    }
    // Wait for all threads to finish
    for (int i = 0; i < NUM_THREADS; i++)
    {
        WaitForSingleObject(*unpack_HANDLE(vec.at(&vec, i)), INFINITE);
    }
    MEM_DEBUG_INSPECT(stderr);

    // Clean up
    vec.free_obj = CloseHandle;
    (vec.free)(&vec);
    // Check for memory leaks
    MEM_DEBUG_END(stderr);

    MEM_DEBUG_INSPECT(stderr);

    return 0;
}
