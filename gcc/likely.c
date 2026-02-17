#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <sysexits.h>

#define likely(x)       __builtin_expect(!!(x), 1)
#define unlikely(x)     __builtin_expect(!!(x), 0)

/* 16KB data to ensure L1 Data Cache hits (typically 32KB) */
#define ARR_SIZE    4096U
#define OUTER_LOOPS 200000U

/* Instruction Cache Displacement & Optimization Barriers */
#if defined(__aarch64__)
    /* AArch64: 32 NOPs = 128 bytes. memory clobber to prevent CSEL/CINC */
    #define HEAVY_COLD_PATH() __asm__ volatile (".rept 32\n\t" "nop\n\t" ".endr")
    #define PREVENT_OPTIMIZE() __asm__ volatile ("" : : : "memory")
#elif defined(__x86_64__)
    /* x86_64: 128 NOPs = 128 bytes */
    #define HEAVY_COLD_PATH() __asm__ volatile (".fill 128, 1, 0x90")
    #define PREVENT_OPTIMIZE()
#else
    #error "Unsupported architecture. This benchmark requires x86_64 or AArch64."
#endif

static inline double calc_diff(struct timespec start, struct timespec end)
{
    return (double)(end.tv_sec - start.tv_sec) +
           (double)(end.tv_nsec - start.tv_nsec) / 1e9;
}

int main(void)
{
    int32_t *data;
    volatile uint64_t sum;
    struct timespec start;
    struct timespec end;
    size_t i;
    size_t j;

    data = (int32_t *)malloc(ARR_SIZE * sizeof(int32_t));
    if (data == NULL) {
        fprintf(stderr, "[ERROR] %s:%d - Memory allocation failed: %s\n",
                __FILE__, __LINE__, strerror(errno));
        return EX_OSERR;
    }

    for (i = 0; i < ARR_SIZE; ++i) {
        data[i] = ((rand() % 100 < 95) ? 1 : 0); /* 95% true, 5% false */
    }

    printf("Benchmark: 'likely' vs 'Default' (L1 Hit Scenario)\n");
#if defined(__aarch64__)
    printf("Target: ARM64 (AArch64)\n");
#elif defined(__x86_64__)
    printf("Target: x86_64\n");
#endif
    printf("--------------------------------------------------------\n");

    /* Test 1: Default Behavior */
    sum = 0;
    clock_gettime(CLOCK_MONOTONIC, &start);
    for (i = 0; i < OUTER_LOOPS; ++i) {
        for (j = 0; j < ARR_SIZE; ++j) {
            if (data[j]) {
                ++sum;
                PREVENT_OPTIMIZE();
            } else {
                HEAVY_COLD_PATH();
                --sum;
            }
        }
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    printf("1. Default (No Hint) : %.6f sec\n", calc_diff(start, end));

    /* Test 2: With likely() Hint */
    sum = 0;
    clock_gettime(CLOCK_MONOTONIC, &start);
    for (i = 0; i < OUTER_LOOPS; ++i) {
        for (j = 0; j < ARR_SIZE; ++j) {
            /* hint forces fall-through layout for the hot path */
            if (likely(data[j])) {
                ++sum;
                PREVENT_OPTIMIZE();
            } else {
                HEAVY_COLD_PATH();
                --sum;
            }
        }
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    printf("2. Likely (Optimized): %.6f sec\n", calc_diff(start, end));

    free(data);
    data = NULL;

    return 0;
}
