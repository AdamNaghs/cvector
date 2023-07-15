#ifndef VEC_DEF_H
#define VEC_DEF_H

#define DEFINE_RETURN_TYPE_DEF(type)            \
    typedef struct Return_##type Return_##type; \
                                                \
    struct Return_##type                        \
    {                                           \
        type *data;                             \
        size_t index;                           \
        Vec_Error err;                          \
    };                                          \
                                                \
    type *unpack_##type(Return_##type unpack)

#define DEFINE_VEC_DEF(type, name)                                             \
    typedef struct name name;                                                  \
    typedef Comparison (*Vec_Compare_Func_##type)(const type *, const type *); \
    struct name                                                                \
    {                                                                          \
        type *data;                                                            \
        size_t size;                                                           \
        size_t capacity;                                                       \
        Vec_Compare_Func_##type compare_vals;                                  \
        bool (*empty)(name * this);                                            \
        size_t (*read_size)(name * this);                                      \
        size_t (*read_capacity)(name * this);                                  \
        Vec_Error (*free)(name * this);                                        \
        Vec_Error (*clear)(name * this);                                       \
        Vec_Error (*compact)(name * this);                                     \
        Vec_Error (*remove)(name * this, size_t index);                        \
        Vec_Error (*realloc)(name * this, size_t capacity);                    \
        Vec_Error (*push_back)(name * this, type value);                       \
        Vec_Error (*insert)(name * this, size_t index, type value);            \
        Vec_Error (*swap)(name * this, size_t index0, size_t index1);          \
        Return_##type (*find)(name * this, type value);                        \
        Return_##type (*at)(name * this, size_t index);                        \
    };                                                                         \
    Vec_Error vec_realloc_##name(name *v, size_t capacity);                    \
    Vec_Error vec_try_resize_##name(name *v);                                  \
    Vec_Error vec_oob_check_##name(name *v, size_t index);                     \
    Vec_Error vec_swap##name(name *v, size_t index0, size_t index1);           \
    Vec_Error vec_clear_##name(name *v);                                       \
    Vec_Error vec_compact_##name(name *v);                                     \
    Vec_Error vec_free_##name(name *v);                                        \
    size_t vec_size_##name(name *v);                                           \
    size_t vec_capacity_##name(name *v);                                       \
    bool vec_empty_##name(name *v);                                            \
    Vec_Error vec_push_back_##name(name *v, type value);                       \
    Vec_Error vec_insert_##name(name *v, size_t index, type value);            \
    Vec_Error vec_remove_##name(name *v, size_t index);                        \
    Return_##type vec_find_##name(name *v, type value);                        \
    Return_##type vec_at_##name(name *v, size_t index);                        \
    Vec_Error name##_init(name *v, Vec_Compare_Func_##type compare_values);    \
    name create_##name(Vec_Compare_Func_##type compare_values);

#endif /* VEC_DEF_H */