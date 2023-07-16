#include <windows.h>
#include <stdio.h>
#include "../mem_debug.h"

#define NUM_THREADS 5
#define NUM_ALLOCATIONS 10
#define ALLOCATION_SIZE 100

// Thread function
DWORD WINAPI allocateMemory(LPVOID param) {
    for (int i = 0; i < NUM_ALLOCATIONS; i++) {
        // Allocate memory but do not free it
        char* memory = (char*) malloc(ALLOCATION_SIZE);
        if (memory == NULL) {
            printf("Failed to allocate memory\n");
            return 1;
        }
        // Uncomment the following line to fix the memory leak
        // free(memory);
    }
    return 0;
}


int main() {
    // Create an array of thread handles
    HANDLE threads[NUM_THREADS];

    // Create the threads
    for (int i = 0; i < NUM_THREADS; i++) {
        threads[i] = CreateThread(NULL, 0, allocateMemory, NULL, 0, NULL);
        if (threads[i] == NULL) {
            printf("Failed to create thread\n");
            return 1;
        }
    }

    // Wait for all threads to finish
    for (int i = 0; i < NUM_THREADS; i++) {
        WaitForSingleObject(threads[i], INFINITE);
    }

    // Clean up
    for (int i = 0; i < NUM_THREADS; i++) {
        CloseHandle(threads[i]);
    }

    // Check for memory leaks
    MEM_DEBUG_END(stderr);

    return 0;
}
