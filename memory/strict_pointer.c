#include <stdio.h>

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

static int scale(double *const restrict mut_dst, const double *const restrict src, double factor,
                 size_t count)
{
    if (count == 0) {
        return -1;
    }

    for (size_t i = 0; i < count; ++i) {
        mut_dst[i] = src[i] * factor;
    }

    return 0;
}

static void set_value(int *const restrict mut_p, int *const mut_q)
{
    *mut_p = 1;
    *mut_q = 2;

    printf("*p is %d\n", *mut_p);
}

int main(void)
{
    double src[5] = { 1.0, 2.0, 3.0, 4.0, 5.0 };
    double dst[5] = { 0 };

    int x = 0;
    int *const ptr_aliased = &x;
    *ptr_aliased = 100;

    scale(dst, src, 10.0, ARRAY_SIZE(src));
    for (size_t i = 0; i < ARRAY_SIZE(dst); ++i) {
        printf("dst[%zu] = %lf\n", i, dst[i]);
    }

    /* UB: violates the restrict contract */
    set_value(&x, ptr_aliased);

    return 0;
}
