#include "mem_debug.h"

Vec_Mem debug_vec;

int main()
{

    init_leak_finder();

    int* i = malloc(sizeof(int));

    free(i);
    find_leaks();
    return 0;

}