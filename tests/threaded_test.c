#include <windows.h>
#include <stdio.h>
#include "../mem_debug.h"


DWORD WINAPI allocateMemory(LPVOID param) {
    int i;
    for (i = 0; i < 10; i++) {
        
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
    
    HANDLE thread1 = CreateThread(NULL, 0, allocateMemory, NULL, 0, NULL);
    HANDLE thread2 = CreateThread(NULL, 0, allocateMemory, NULL, 0, NULL);

    
    WaitForSingleObject(thread1, INFINITE);
    WaitForSingleObject(thread2, INFINITE);

    
    CloseHandle(thread1);
    CloseHandle(thread2);


    MEM_DEBUG_END(stderr);
    return 0;
}
