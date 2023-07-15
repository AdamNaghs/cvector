#include "../mem_debug.h"


DEFINE_VEC(int, Vec_Int);

void alloc_test(uint8 depth)
{
    if (depth > 5) return;
    void* test = malloc(10);
    alloc_test(depth+1);

}

MEM_DEBUG_INIT;

int main()
{
    MEM_DEBUG_START();

    Vec_Int v;
    CREATE_VEC(&v, compare_ints, int, Vec_Int);
    Vec_Int *vec = &v;

    vec->push_back(vec, 1);

    alloc_test(0);
    MEM_DEBUG_INSPECT(stderr);
    int *i = malloc(sizeof(int));
    int k;
    int *j = &k;
    free(i);
    (vec->free)(vec);
    //free(j);

    MEM_DEBUG_END();
    return 0;
}