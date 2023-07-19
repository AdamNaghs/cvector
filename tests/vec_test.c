#include "../util.h"
#include "../vec.h"

DEFINE_VEC(int, Vec_int, compare_ints);

void print_vec_int(Vec_int *v)
{
    int i;
    for (i = 0; i < v->size; i++)
    {
        fprintf(stderr, "%d ", *(v->at(v, i).data));
    }
    fprintf(stderr, "\n");
}

int main()
{
    Vec_int v;
    Vec_Error init_err = Vec_int_init(&v, NULL);
    ASSERT_PRINT(init_err == VEC_OK);

    int i;
    for (i = 0; i < 10; i++)
    {

        Vec_Error push_back_err = v.push_back(&v, i);
        ASSERT_PRINT(push_back_err == VEC_OK);
        ASSERT_PRINT(v.data[i] == i);
        fprintf(stderr, "Capacity %llu, Size %llu, Index/data %d\n", v.capacity, v.size, i);
    }
    print_vec_int(&v);

    size_t size = v.read_size(&v);
    ASSERT_PRINT(size != SIZE_MAX);
    ASSERT_PRINT(size == 10);
    fprintf(stderr, "Read size after adding %llu\n", size);

    size_t capacity_after_adds = v.read_capacity(&v);
    ASSERT_PRINT(capacity_after_adds != SIZE_MAX);
    ASSERT_PRINT(capacity_after_adds == 16);

    Vec_Error compact_err = v.compact(&v);
    ASSERT_PRINT(compact_err == VEC_OK);
    size_t capacity_after_compact = v.read_capacity(&v);
    ASSERT_PRINT(capacity_after_compact != SIZE_MAX);
    ASSERT_PRINT(capacity_after_compact == 10);
    print_vec_int(&v);

    for (i = 9; i >= 0; i--)
    {

        Vec_Error remove_err = v.remove(&v, i);
        ASSERT_PRINT(remove_err == VEC_OK);
        ASSERT_PRINT(v.data[i] == i);
        fprintf(stderr, "Capacity %llu, Size %llu, Index/data %d\n", v.capacity, v.size, i);
    }

    Vec_Error clear_err = v.clear(&v);
    ASSERT_PRINT(clear_err == VEC_OK);

    print_vec_int(&v);

    size_t capacity_after_clear = v.read_capacity(&v);
    ASSERT_PRINT(capacity_after_clear != SIZE_MAX);
    ASSERT_PRINT(capacity_after_clear == 2);

    size_t size_after_clear = v.read_size(&v);
    ASSERT_PRINT(size_after_clear != SIZE_MAX);
    ASSERT_PRINT(size_after_clear == 0);
    fprintf(stderr, "Read size after clear %llu\n", size_after_clear);

    Vec_Error compact_after_clear_err = v.compact(&v);
    fprintf(stderr, "compact_after_clear_err Vec_Error %d\n", compact_after_clear_err);
    ASSERT_PRINT(compact_after_clear_err == VEC_OK);
    size_t capacity_after_compact_and_clear = v.read_capacity(&v);
    ASSERT_PRINT(capacity_after_compact_and_clear != SIZE_MAX);
    ASSERT_PRINT(capacity_after_compact_and_clear == 1);

    Vec_Error insert_err = v.insert(&v, 0, 5);
    ASSERT_PRINT(insert_err == VEC_OK);
    ASSERT_PRINT(v.data[0] == 5);

    print_vec_int(&v);

    Return_int find_res = v.find(&v, 5);
    ASSERT_PRINT(find_res.err == VEC_OK);
    ASSERT_PRINT(*find_res.data == 5);
    ASSERT_PRINT(find_res.index == 0);

    Return_int at_res = v.at(&v, 0);
    ASSERT_PRINT(at_res.err == VEC_OK);
    ASSERT_PRINT(*at_res.data == 5);
    ASSERT_PRINT(at_res.index == 0);

    v.push_back(&v, 7);
    v.push_back(&v, 8);
    print_vec_int(&v);
    fprintf(stderr, "Capacity at end %llu, Size at end %llu\n", v.capacity, v.size);
    ASSERT_PRINT(v.capacity == 4);

    Vec_Error free_err = v.free(&v);
    ASSERT_PRINT(free_err == VEC_OK);

    bool empty = v.empty(&v);
    ASSERT_PRINT(empty == true);

    return 0;
}