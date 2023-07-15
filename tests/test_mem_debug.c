#include "../mem_debug.h"

DEFINE_VEC(int, Vec_Int);

MEM_DEBUG_INIT;

int main()
{
    MEM_DEBUG_START();

    Vec_Int v;
    CREATE_VEC(&v, compare_ints, int, Vec_Int);
    Vec_Int *vec = &v;

    vec->push_back(&v, 1);

    int *i = malloc(sizeof(int));
    int k;

    // free(k);
    free(i);
    (vec->free)(vec);
    MEM_DEBUG_END();

    return 0;
}