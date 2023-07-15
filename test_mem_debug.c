#include "mem_debug.h"

Vec_Mem debug_vec;

int main()
{

    init_leak_finder();

    int* i = malloc(sizeof(int));

    find_leaks();
    return 0;

}