#include <windows.h>
#include <stdio.h>
#include "../mem_debug.h"

// Thread function
DWORD WINAPI allocateMemory(LPVOID param) {
    for (int i = 0; i < 10; i++) {
        // Allocate memory but do not free it
        char* memory = (char*) malloc(100);
        if (memory == NULL) {
            printf("Failed to allocate memory\n");
            return 1;
        }
        free(memory);
    }
    return 0;
}

int main() {
    // Create two threads
    HANDLE thread1 = CreateThread(NULL, 0, allocateMemory, NULL, 0, NULL);
    HANDLE thread2 = CreateThread(NULL, 0, allocateMemory, NULL, 0, NULL);

    // Wait for both threads to finish
    WaitForSingleObject(thread1, INFINITE);
    WaitForSingleObject(thread2, INFINITE);

    // Clean up
    CloseHandle(thread1);
    CloseHandle(thread2);


    MEM_DEBUG_END(stderr);
    return 0;
}
