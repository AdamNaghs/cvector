#ifndef _STACK_H
#define _STACK_H

#include <assert.h>
#include <stdlib.h>

#define STACK_ASSERT_IF_NULL(ptr) assert(ptr != NULL);

/* these were written before logger was created and does not log to file */
#define STACK_ASSERT_IF_NULL_LOG(ptr, fstring, ...) \
    do                                        \
    {                                         \
        if (!ptr)                             \
        {                                     \
            printf(fstring, __VA_ARGS__);     \
            assert(ptr != NULL);              \
        }                                     \
    } while (0);

#define STACK_ASSERT_LOG(cond, fstring, ...)    \
    do                                    \
    {                                     \
        if (!cond)                        \
        {                                 \
            printf(fstring, __VA_ARGS__); \
            assert(cond);                 \
        }                                 \
    } while (0);

/* Functions:
    this Create (size)
    void Destroy (this)
    void Reisze (this, size)
    void Push (this, value)
    value Pop (this)
    int/bool IsEmpty (this)
*/
#define DEFINE_STACK(type, stack_name)                                \
    typedef struct                                                    \
    {                                                                 \
        type *data;                                                   \
        size_t cap;                                                   \
        signed int top;                                               \
    } stack_name;                                                     \
                                                                      \
    stack_name *stack_name##Create(size_t _cap)                       \
    {                                                                 \
        stack_name *s = (stack_name *)malloc(sizeof(stack_name));     \
        STACK_ASSERT_IF_NULL(s);                                            \
        s->data = (type *)calloc(_cap, sizeof(type));                 \
        s->cap = _cap;                                                \
        s->top = -1;                                                  \
        return s;                                                     \
    }                                                                 \
                                                                      \
    void stack_name##Destroy(stack_name *s)                           \
    {                                                                 \
        STACK_ASSERT_IF_NULL(s);                                            \
        free(s->data);                                                \
        free(s);                                                      \
    }                                                                 \
                                                                      \
    void stack_name##Resize(stack_name *s, size_t _cap)               \
    {                                                                 \
        STACK_ASSERT_IF_NULL(s);                                            \
        s->cap = _cap;                                                \
        type *resize = (type *)realloc(s->data, sizeof(type) * _cap); \
        STACK_ASSERT_IF_NULL(resize);                                       \
        s->data = resize;                                             \
    }                                                                 \
                                                                      \
    void stack_name##Push(stack_name *s, type v)                      \
    {                                                                 \
        STACK_ASSERT_IF_NULL(s);                                            \
        if (s->top == s->cap - 1)                                     \
            stack_name##Resize(s, s->cap *= 2);                       \
        s->data[++s->top] = v;                                        \
    }                                                                 \
                                                                      \
    type stack_name##Pop(stack_name *s)                               \
    {                                                                 \
        STACK_ASSERT_IF_NULL(s);                                            \
        assert(s->top > -1);                                          \
        return s->data[s->top--];                                     \
    }                                                                 \
    int stack_name##IsEmpty(stack_name *s) { return s->top == -1; }

#endif /* _STACK_H */