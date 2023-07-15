#include "mem_debug.h"

Vec_Mem debug_vec;


DEFINE_VEC(int,Vec_Int);

int main()
{
    init_leak_finder();

    Vec_Int v ;//= create_Vec_Int(compare_ints);
    CREATE_VEC(&v,compare_ints,int,Vec_Int);

    v.push_back(&v,1);

    

    int* i = malloc(sizeof(int));
    int k;


    //free(k);
    find_leaks();
    //free(i);

    return 0;

}