#include "../stack.h"
#include <stdio.h>
/* test */
DEFINE_STACK(int, iStack);

int main(void)
{
    int size = 10;
    iStack *s = iStackCreate(size);
    int i;
    for (i = 0; i < size; i++)
    {
        iStackPush(s, i);
        printf("i = %d, top = %d, cap = %d\n", i, s->top, s->cap);
    }
    iStackPush(s, 5);
    printf("5 == %d, top = %d, cap = %d\n", iStackPop(s), s->top, s->cap);
    printf("top : 9 == %d\n", s->top);
    while (!iStackIsEmpty(s))
        printf("%d == %d, top = %d\n", --i, iStackPop(s), s->top);
    iStackDestroy(s);
    return 0;
}