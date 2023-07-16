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


Comparison MyCompareObjectHandles(HANDLE const * hFirst, HANDLE const * hSecond) {
    return (hFirst == hSecond) ? EQUAL : LESS;
}

DEFINE_VEC(HANDLE, Vec_Thread, MyCompareObjectHandles);

int main() {
    // Create an array of thread handles
    //HANDLE threads[NUM_THREADS];

    Vec_Thread vec;
    CREATE_VEC(&vec,MyCompareObjectHandles,HANDLE,Vec_Thread);
    // Create the threads
    for (int i = 0; i < NUM_THREADS; i++) {
        //threads[i] = CreateThread(NULL, 0, allocateMemory, NULL, 0, NULL);
        //if (threads[i] == NULL) {
        //    printf("Failed to create thread\n");
        //    return 1;
        //}
        vec.push_back(&vec,CreateThread(NULL, 0, allocateMemory, NULL, 0, NULL));
    }
    // Wait for all threads to finish
    for (int i = 0; i < NUM_THREADS; i++) {
        //WaitForSingleObject(threads[i], INFINITE);
        WaitForSingleObject(*unpack_HANDLE(vec.at(&vec,i)), INFINITE);
    }
    MEM_DEBUG_INSPECT(stderr);

    // Clean up
    for (int i = 0; i < NUM_THREADS; i++) {
        //CloseHandle(threads[i]);
        CloseHandle(*(vec.at(&vec,i).data));
    }

    // Check for memory leaks
    MEM_DEBUG_END(stderr);

    return 0;
}
