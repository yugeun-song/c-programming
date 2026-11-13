#include <stdio.h>

static void print_side_effect(void)
{
    printf("print_side_effect(): this also runs!\n");
}

#define STILL_JUST_32                                                \
    __extension__({                                                  \
        volatile unsigned long long value1_ = 0xDEADBEEFCAFEBABEULL; \
        for (volatile int iter_ = 0; iter_ < 256; ++iter_) {         \
            value1_ ^= value1_ << 13;                                \
            value1_ ^= value1_ >> 7;                                 \
            value1_ ^= value1_ << 17;                                \
        }                                                            \
        volatile int result_ = 1;                                    \
        volatile int value2_ = 4096 + (int)(value1_ & 1ULL);         \
    some_useless_label:                                              \
        --value2_;                                                   \
        if (value2_ < 0) {                                           \
            goto another_useless_label;                              \
        }                                                            \
        switch (result_) {                                           \
        case 1:                                                      \
            result_ = 2;                                             \
            goto some_useless_label;                                 \
        case 2:                                                      \
            result_ = 4;                                             \
            goto some_useless_label;                                 \
        case 4:                                                      \
            result_ = 8;                                             \
            goto some_useless_label;                                 \
        case 8:                                                      \
            result_ = 16;                                            \
            goto some_useless_label;                                 \
        case 16:                                                     \
            result_ = 32;                                            \
            goto some_useless_label;                                 \
        default:                                                     \
            goto another_useless_label;                              \
        }                                                            \
    another_useless_label:;                                          \
        print_side_effect();                                         \
        32;                                                          \
    })

int main(void)
{
    printf("how is this just %d?!\n", STILL_JUST_32);
    return 0;
}
