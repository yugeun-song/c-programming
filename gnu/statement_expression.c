#include <stdio.h>

int some_foo(void)
{
    printf("some_foo(): this also runs!\n");
    return 0;
}

#define STILL_JUST_32                                                                              \
    __extension__({                                                                                \
        volatile unsigned long long some_value1 = 0xDEADBEEFCAFEBABEULL;                           \
        for (volatile int iter = 0; iter < 256; ++iter) {                                          \
            some_value1 ^= some_value1 << 13;                                                      \
            some_value1 ^= some_value1 >> 7;                                                       \
            some_value1 ^= some_value1 << 17;                                                      \
        }                                                                                          \
        volatile int some_result = 1;                                                              \
        volatile int some_value2 = 4096 + (int)(some_value1 & 1ULL);                               \
    some_useless_label:                                                                            \
        --some_value2;                                                                             \
        if (some_value2 < 0) {                                                                     \
            goto another_useless_label;                                                            \
        }                                                                                          \
        switch (some_result) {                                                                     \
        case 1:                                                                                    \
            some_result = 2;                                                                       \
            goto some_useless_label;                                                               \
        case 2:                                                                                    \
            some_result = 4;                                                                       \
            goto some_useless_label;                                                               \
        case 4:                                                                                    \
            some_result = 8;                                                                       \
            goto some_useless_label;                                                               \
        case 8:                                                                                    \
            some_result = 16;                                                                      \
            goto some_useless_label;                                                               \
        case 16:                                                                                   \
            some_result = 32;                                                                      \
            goto some_useless_label;                                                               \
        default:                                                                                   \
            goto another_useless_label;                                                            \
        }                                                                                          \
    another_useless_label:;                                                                        \
        some_foo();                                                                                \
        32;                                                                                        \
    })

int main(void)
{
    printf("how is this just %d?!\n", STILL_JUST_32);
    return 0;
}