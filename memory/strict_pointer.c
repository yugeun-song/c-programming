#include <stdio.h>

static void set_value(int *restrict p, int *q)
{
    *p = 1;
    *q = 2;

    printf("%d\n", *p);
}

int main(void)
{
    int x = 0;
    int *ptr_aliased = &x;
    *ptr_aliased = 100;

    /* UB: violates the restrict contract */
    set_value(&x, ptr_aliased);

    return 0;
}