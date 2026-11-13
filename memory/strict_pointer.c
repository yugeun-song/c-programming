#include <stdio.h>

static int scale(double *restrict dst, double *restrict src, double factor, int n)
{
    if (n <= 0) {
        return -1;
    }

    for (int i = 0; i < n; ++i) {
        dst[i] = src[i] * factor;
    }

    return 0;
}

static void set_value(int *restrict p, int *q)
{
    *p = 1;
    *q = 2;

    printf("*p is %d\n", *p);
}

int main(void)
{
    double src[5] = { 1.0, 2.0, 3.0, 4.0, 5.0 };
    double dst[5] = { 0 };

    int x = 0;
    int *ptr_aliased = &x;
    *ptr_aliased = 100;

    scale(dst, src, 10, 5);
    for (int i = 0; i < 5; ++i) {
        printf("dst[%d] = %lf\n", i, dst[i]);
    }

    /* UB: violates the restrict contract */
    set_value(&x, ptr_aliased);

    return 0;
}