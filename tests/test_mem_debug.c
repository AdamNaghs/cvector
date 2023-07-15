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
    //Vec_Int_init(&v,compare_ints);
    v.push_back(&v,1);

    alloc_test(0);
    int *i = malloc(sizeof(int));
    int k;
    int *j = &k;
    //free(j);
    v.push_back(&v,2);
    //fprintf(stderr,"%d\n",(v.read_capacity(&v)));
    //MEM_DEBUG_INSPECT(stderr);

    v.push_back(&v,3);
    //fprintf(stderr,"%d\n",(v.read_capacity(&v)));
    //MEM_DEBUG_INSPECT(stderr);

    //free(i);
    (v.free)(&v);
    MEM_DEBUG_END();
    return 0;
}